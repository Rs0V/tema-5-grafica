#pragma once

#include "datatypes.hpp"

///////////////////////////////////////////////////////////////////////////

struct Object {
	u32 VAO = 0, VBO = 0, EBO = 0, COLOR = 0, NORMAL = 0, UV = 0, INST = 0, PROGRAM = 0, TEX = 0;
	varray<f32> vertices, colors, normals, uvs;
	varray<m4> instances;
	varray<u32> indices;
	f3 centre = f4(0.0f);
	u32 draw_mode;
	bool visible = true;

	m4 original_matrix{ 0.0f };
	f3 origin{ 0.0f, 0.0f, 0.0f };
	f3 location{ 0.0f, 0.0f, 0.0f };
	f3 rotation{ 0.0f, 0.0f, 0.0f };
	f3 scale{ 1.0f, 1.0f, 1.0f };

	f3 forward{ 0.0f, 0.0f, -1.0f };
	f3 right{ 1.0f, 0.0f, 0.0f };
	f3 up{ 0.0f, 1.0f, 0.0f };



	void use() const {
		glUseProgram(this->PROGRAM);
		glBindVertexArray(this->VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->TEX);

		glUniform1i(glGetUniformLocation(this->PROGRAM, "myTexture"), 0);
	}

	void draw() const {
		this->use();
		if (this->visible == true) {
			glDrawElementsInstanced(this->draw_mode, this->indices.size(), GL_UNSIGNED_INT, (void*)0, this->instances.size());
		}
	}

	i32 get_uniform(str name) const {
		return glGetUniformLocation(this->PROGRAM, name.data());
	}
	template<class T, class U = f32, u32 size = 1> array<U, size> get_uniform_value(str name) const;

	#pragma region void set_uniform(str name, type value) const
	#define define_set_uniform(type) \
	void set_uniform(str name, type value) const

	define_set_uniform(u32);
	define_set_uniform(u1);
	define_set_uniform(u2);
	define_set_uniform(u3);
	define_set_uniform(u4);

	define_set_uniform(i32);
	define_set_uniform(i1);
	define_set_uniform(i2);
	define_set_uniform(i3);
	define_set_uniform(i4);

	define_set_uniform(f32);
	define_set_uniform(f1);
	define_set_uniform(f2);
	define_set_uniform(f3);
	define_set_uniform(f4);

	define_set_uniform(f64);
	define_set_uniform(d1);
	define_set_uniform(d2);
	define_set_uniform(d3);
	define_set_uniform(d4);

	define_set_uniform(m4);
	#pragma endregion

	void transform(maybe<f3> location, maybe<f3> rotation, maybe<f3> scale) const;
	void transform_mutable(maybe<f3> location, maybe<f3> rotation, maybe<f3> scale);

	void transform_delta(maybe<f3> delta_location, maybe<f3> delta_rotation, maybe<f3> delta_scale) const;
	void transform_delta_mutable(maybe<f3> delta_location, maybe<f3> delta_rotation, maybe<f3> delta_scale);

	void set_matrix(m4 matrix) {
		this->use();
		this->set_uniform("myMatrix", matrix);
		this->original_matrix = matrix;
	}

	void set_origin(f3 origin) {
		this->origin = origin;
	}

	f3 get_real_location() const;
};


extern umap<str, Object> objects;
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
);

void destroy_object(Object &obj);

void make_instance(str obj_name, u32 number, varray<f32> &&colors, varray<m4> &&instances);
