#include "object.hpp"
#include "shader.hpp"
#include "app.hpp"
#include "utils.hpp"

///////////////////////////////////////////////////////////////////////////

umap<str, Object> objects{};
void create_object(
	str obj_name,
	varray<f32> &&vertices,
	varray<f32> &&colors,
	varray<f32> &&normals,
	varray<u32> &&indices,
	varray<f32> &&uvs,
	maybe<u32> draw_mode,
	maybe<str> vertex_shader_path,
	maybe<str> fragment_shader_path,
	maybe<str> texture_path,
	maybe<bool> has_opacity
) {
	vdefaults(vertices, 0.0f, 0.0f, 0.0f);
	vfordef(colors, vertices.size() / 3, 1.0f, 1.0f, 1.0f, 1.0f);
	vfordef(normals, vertices.size() / 3, 0.0f, 0.0f, 0.0f);
	vfordef(uvs, vertices.size() / 3, 1.0f, 1.0f);
	vdefaults(indices, 0);

	defaults(draw_mode, GL_TRIANGLES);
	defaults(vertex_shader_path, "default.vert");
	defaults(fragment_shader_path, "default.frag");
	defaults(texture_path, "");
	defaults(has_opacity, false);

	Object obj;
	obj.draw_mode = draw_mode.value();

	glGenVertexArrays(1, &obj.VAO);
	glBindVertexArray(obj.VAO);

	glGenBuffers(1, &obj.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof vertices[0], vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &obj.COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, obj.COLOR);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof colors[0], colors.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(1, 1);

	glGenBuffers(1, &obj.NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, obj.NORMAL);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof normals[0], normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &obj.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof indices[0], indices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &obj.UV);
	glBindBuffer(GL_ARRAY_BUFFER, obj.UV);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof uvs[0], uvs.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	obj.vertices = vertices;
	obj.colors = colors;
	obj.normals = normals;
	obj.indices = indices;
	obj.uvs = uvs;


	obj.instances.emplace_back(m4(1.0f));
	glGenBuffers(1, &obj.INST);
	glBindBuffer(GL_ARRAY_BUFFER, obj.INST);
	glBufferData(GL_ARRAY_BUFFER, obj.instances.size() * sizeof obj.instances[0], obj.instances.data(), GL_STATIC_DRAW);

	for (u32 i = 0; i < 4; i++) {
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof m4, (void*)(sizeof f4 * i));
		glVertexAttribDivisor(4 + i, 1);
	}


	for (u32 i = 0; i < obj.vertices.size(); i += 3) {
		obj.centre.x += obj.vertices[i];
		obj.centre.y += obj.vertices[i + 1];
		obj.centre.z += obj.vertices[i + 2];
	}
	obj.centre /= obj.vertices.size() / 3;

	create_shaders(obj, vertex_shader_path, fragment_shader_path);

	if (texture_path.value().empty() == false) {
		load_texture(obj, texture_path.value(), has_opacity.value());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, obj.TEX);

		obj.set_uniform("myTexture", 0);
		obj.set_uniform("useTex", 1);
	}
	else {
		obj.set_uniform("useTex", 0);
	}
	
	objects.insert({ obj_name, obj });
}

void destroy_object(Object &obj) {
	obj.use();

	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &obj.INST);
	glDeleteBuffers(1, &obj.UV);
	glDeleteBuffers(1, &obj.EBO);
	glDeleteBuffers(1, &obj.NORMAL);
	glDeleteBuffers(1, &obj.COLOR);
	glDeleteBuffers(1, &obj.VBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &obj.VAO);
}

void make_instance(str obj_name, u32 number, varray<f32> &&colors, varray<m4> &&instances) {
	Object &obj = objects[obj_name];
	obj.use();


	f4 median_color = f4(
		obj.colors[0],
		obj.colors[1],
		obj.colors[2],
		obj.colors[3]
	);
	for (u32 i = 0; i < obj.colors.size() / 4; i++) {
		median_color.r = (median_color.r + obj.colors[i * 4 + 0]) / 2.0f;
		median_color.g = (median_color.g + obj.colors[i * 4 + 1]) / 2.0f;
		median_color.b = (median_color.b + obj.colors[i * 4 + 2]) / 2.0f;
		median_color.a = (median_color.a + obj.colors[i * 4 + 3]) / 2.0f;
	}
	obj.colors.resize(4);
	obj.colors.reserve((obj.instances.size() + number) * 4);

	if (colors.empty() == true) {
		for (u32 i = 0; i < number; i++) {
			obj.colors.insert(obj.colors.end(), obj.colors.begin(), obj.colors.begin() + 4);
		}
	}
	else {
		obj.colors.insert(obj.colors.end(), colors.begin(), colors.end());
	}

	glBindBuffer(GL_ARRAY_BUFFER, obj.COLOR);
	glBufferData(GL_ARRAY_BUFFER, obj.colors.size() * sizeof obj.colors[0], obj.colors.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(1, 1);



	obj.instances.reserve(obj.instances.size() + number);
	if (instances.empty() == true) {
		obj.instances.insert(obj.instances.end(), number, m4(1.0f));
	}
	else {
		obj.instances.insert(obj.instances.end(), instances.begin(), instances.end());
	}

	glBindBuffer(GL_ARRAY_BUFFER, obj.INST);
	glBufferData(GL_ARRAY_BUFFER, obj.instances.size() * sizeof obj.instances[0], obj.instances.data(), GL_STATIC_DRAW);

	for (u32 i = 0; i < 4; i++) {
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof m4, (void*)(sizeof f4 * i));
		glVertexAttribDivisor(4 + i, 1);
	}
}



#define set_uni(type, func) \
void Object::set_uniform(str name, type value) const { \
	this->use(); \
	func(this->get_uniform(name), value); \
}

#define set_uni1(type, func) \
void Object::set_uniform(str name, type value) const { \
	this->use(); \
	func(this->get_uniform(name), value.x); \
}

#define set_uni2(type, func) \
void Object::set_uniform(str name, type value) const { \
	this->use(); \
	func(this->get_uniform(name), value.x, value.y); \
}

#define set_uni3(type, func) \
void Object::set_uniform(str name, type value) const { \
	this->use(); \
	func(this->get_uniform(name), value.x, value.y, value.z); \
}

#define set_uni4(type, func) \
void Object::set_uniform(str name, type value) const { \
	this->use(); \
	func(this->get_uniform(name), value.x, value.y, value.z, value.w); \
}

set_uni(u32, glUniform1ui)
set_uni1(u1, glUniform1ui)
set_uni2(u2, glUniform2ui)
set_uni3(u3, glUniform3ui)
set_uni4(u4, glUniform4ui)

set_uni(i32, glUniform1i)
set_uni1(i1, glUniform1i)
set_uni2(i2, glUniform2i)
set_uni3(i3, glUniform3i)
set_uni4(i4, glUniform4i)

set_uni(f32, glUniform1f)
set_uni1(f1, glUniform1f)
set_uni2(f2, glUniform2f)
set_uni3(f3, glUniform3f)
set_uni4(f4, glUniform4f)

set_uni(f64, glUniform1d)
set_uni1(d1, glUniform1d)
set_uni2(d2, glUniform2d)
set_uni3(d3, glUniform3d)
set_uni4(d4, glUniform4d)

void Object::set_uniform(str name, m4 value) const {
	this->use();
	glUniformMatrix4fv(this->get_uniform(name), 1, GL_FALSE, &value[0][0]);
}



#define get_uni_val(type, size, func) \
template<> array<type, size> Object::get_uniform_value<type, type, size>(str name) const { \
	array<type, size> values; \
	func(this->PROGRAM, this->get_uniform(name), values.data()); \
	\
	return values; \
}

get_uni_val(u32, 1, glGetUniformuiv)
get_uni_val(u32, 2, glGetUniformuiv)
get_uni_val(u32, 3, glGetUniformuiv)
get_uni_val(u32, 4, glGetUniformuiv)

get_uni_val(i32, 1, glGetUniformiv)
get_uni_val(i32, 2, glGetUniformiv)
get_uni_val(i32, 3, glGetUniformiv)
get_uni_val(i32, 4, glGetUniformiv)

get_uni_val(f32, 1, glGetUniformfv)
get_uni_val(f32, 2, glGetUniformfv)
get_uni_val(f32, 3, glGetUniformfv)
get_uni_val(f32, 4, glGetUniformfv)

get_uni_val(f64, 1, glGetUniformdv)
get_uni_val(f64, 2, glGetUniformdv)
get_uni_val(f64, 3, glGetUniformdv)
get_uni_val(f64, 4, glGetUniformdv)

template<> array<f32, 4> Object::get_uniform_value<m4>(str name) const {
	array<f32, 4> values;
	glGetUniformfv(this->PROGRAM, this->get_uniform(name), values.data());
	
	return values;
}



tarray(f3, 3) Object::transform(maybe<f3> location, maybe<f3> rotation, maybe<f3> scale) const {
	this->use();

	defaults(location, this->location);
	defaults(rotation, this->rotation);
	defaults(scale,    this->scale);


	f3 old_location = this->location;
	f3 old_rotation = this->rotation;
	f3 old_scale    = this->scale;

	f3 rot_f3 = glm::radians(rotation.value());
	m4 rot_m4 = m4(1.0f)
		* glm::translate(m4(1.0f), this->origin)

		* glm::rotate(m4(1.0f), rot_f3.x, f3(1.0f, 0.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.y, f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.z, f3(0.0f, 0.0f, 1.0f))

		* glm::translate(m4(1.0f), -this->origin)
	;
	f3 new_location = f3(rot_m4 * f4(location.value(), 1.0f));
	f3 new_rotation = rotation.value();
	f3 new_scale    = scale.value();


	m4 new_matrix = this->original_matrix
		* glm::translate(m4(1.0f), new_location)

		* glm::rotate(m4(1.0f), glm::radians(new_rotation.x), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(new_rotation.y), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(new_rotation.z), f3(0.0f, 1.0f, 0.0f))

		* glm::scale(m4(1.0f), new_scale)
	;

	this->set_uniform("myMatrix", new_matrix);
	return { new_location - old_location, new_rotation - old_rotation, new_scale / old_scale };
}

tarray(f3, 3) Object::transform_mutable(maybe<f3> location, maybe<f3> rotation, maybe<f3> scale) {
	this->use();

	defaults(location, this->location);
	defaults(rotation, this->rotation);
	defaults(scale,    this->scale);


	f3 old_location = this->location;
	f3 old_rotation = this->rotation;
	f3 old_scale    = this->scale;

	f3 rot_f3 = glm::radians(rotation.value());
	m4 rot_m4 = m4(1.0f)
		* glm::translate(m4(1.0f), this->origin)

		* glm::rotate(m4(1.0f), rot_f3.x, f3(1.0f, 0.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.y, f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.z, f3(0.0f, 0.0f, 1.0f))

		* glm::translate(m4(1.0f), -this->origin)
	;
	this->location = f3(rot_m4 * f4(location.value(), 1.0f));
	this->rotation = rotation.value();
	this->scale    = scale.value();


	m4 new_matrix = this->original_matrix
		* glm::translate(m4(1.0f), this->location)

		* glm::rotate(m4(1.0f), glm::radians(this->rotation.x), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(this->rotation.y), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(this->rotation.z), f3(0.0f, 1.0f, 0.0f))

		* glm::scale(m4(1.0f), this->scale)
	;

	this->set_uniform("myMatrix", new_matrix);
	return { this->location - old_location, this->rotation - old_rotation, this->scale / old_scale };
}

tarray(f3, 3) Object::transform_delta(maybe<f3> delta_location, maybe<f3> delta_rotation, maybe<f3> delta_scale) const {
	this->use();

	defaults(delta_location, f3(0.0f));
	defaults(delta_rotation, f3(0.0f));
	defaults(delta_scale, f3(1.0f));


	f3 old_location = this->location;
	f3 old_rotation = this->rotation;
	f3 old_scale    = this->scale;

	f3 rot_f3 = glm::radians(delta_rotation.value());
	m4 rot_m4 = m4(1.0f)
		* glm::translate(m4(1.0f), this->origin)

		* glm::rotate(m4(1.0f), rot_f3.x, f3(1.0f, 0.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.y, f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.z, f3(0.0f, 0.0f, 1.0f))

		* glm::translate(m4(1.0f), -this->origin)
	;
	f3 new_location = f3(rot_m4 * f4(this->location + delta_location.value(), 1.0f));
	f3 new_rotation = this->rotation + delta_rotation.value();
	f3 new_scale    = this->scale * delta_scale.value();


	m4 new_matrix = this->original_matrix
		* glm::translate(m4(1.0f), new_location)

		* glm::rotate(m4(1.0f), glm::radians(new_rotation.x), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(new_rotation.y), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(new_rotation.z), f3(0.0f, 1.0f, 0.0f))

		* glm::scale(m4(1.0f), new_scale)
	;

	this->set_uniform("myMatrix", new_matrix);
	return { new_location - old_location, delta_rotation.value(), delta_scale.value() };
}

tarray(f3, 3) Object::transform_delta_mutable(maybe<f3> delta_location, maybe<f3> delta_rotation, maybe<f3> delta_scale) {
	this->use();

	defaults(delta_location, f3(0.0f));
	defaults(delta_rotation, f3(0.0f));
	defaults(delta_scale, f3(1.0f));


	f3 old_location = this->location;
	f3 old_rotation = this->rotation;
	f3 old_scale    = this->scale;

	f3 rot_f3 = glm::radians(delta_rotation.value());
	m4 rot_m4 = m4(1.0f)
		* glm::translate(m4(1.0f), this->origin)

		* glm::rotate(m4(1.0f), rot_f3.x, f3(1.0f, 0.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.y, f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), rot_f3.z, f3(0.0f, 0.0f, 1.0f))

		* glm::translate(m4(1.0f), -this->origin)
	;
	this->location  = f3(rot_m4 * f4(this->location + delta_location.value(), 1.0f));
	this->rotation += delta_rotation.value();
	this->scale    *= delta_scale.value();


	m4 new_matrix = this->original_matrix
		* glm::translate(m4(1.0f), this->location)

		* glm::rotate(m4(1.0f), glm::radians(this->rotation.x), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(this->rotation.y), f3(0.0f, 1.0f, 0.0f))
		* glm::rotate(m4(1.0f), glm::radians(this->rotation.z), f3(0.0f, 1.0f, 0.0f))

		* glm::scale(m4(1.0f), this->scale)
	;

	this->set_uniform("myMatrix", new_matrix);
	return { this->location - old_location, delta_rotation.value(), delta_scale.value() };
}