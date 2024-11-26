#include "app.hpp"
#include "object.hpp"
#include "utils.hpp"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////

u32 app::fps = 60;
u32 app::frametime = 1000 / app::fps;

umap<str, bool> app::keys{};
umap<str, bool> app::mouse_keys{};

Camera app::camera{};



void app::create() {
	/*
	constexpr f32
		umin = -glm::half_pi<f32>(),
		umax =  glm::half_pi<f32>(),
		vmin =  0.0f,
		vmax =  glm::two_pi<f32>()
	;
	u32
		M = 20,
		P = 20
	;
	f32
		step_u = (umax - umin) / P,
		step_v = (vmax - vmin) / M
	;
	f32 radius = 40.0f;
	u32 index, index_aux;

	varray<f32> sphere_verts((P + 1) * M);
	varray<f32> sphere_colors((P + 1) * M);
	varray<u32> sphere_indices(2 * M * (P + 1) + 4 * M * (P + 1));

	for (u32 m = 0; m < M; m++) {
		for (u32 p = 0; p < P; p++) {
			f32
				u = umin + p * step_u,
				v = vmin + m * step_v,
				x_vf = radius * glm::cos(u) * glm::cos(v),
				y_vf = radius * glm::cos(u) * glm::sin(v),
				z_vf = radius * glm::sin(u)
			;

			index = m * (P + 1) + p;
			vassign3(sphere_verts[index], f3(x_vf, y_vf, z_vf));
			vassign3(
				sphere_colors[index],
				f3(
					0.1f + glm::sin(u),
					0.1f + glm::cos(v),
					0.1f + glm::sin(u) * -1.5f
				)
			);
			sphere_indices[index] = index;

			index_aux = p * M + m;
			sphere_indices[(P + 1) * M + index_aux] = index;

			if ((p + 1) % (P + 1) != 0) {
				u32
					AUX    = 2 * (P + 1) * M,
					index1 = index,
					index2 = index  + (P + 1),
					index3 = index2 + 1,
					index4 = index  + 1
				;

				if (m == M - 1) {
					index2 = index2 % (P + 1);
					index3 = index3 % (P + 1);
				}

				sphere_indices[AUX + 4 * index + 0] = index1;
				sphere_indices[AUX + 4 * index + 1] = index2;
				sphere_indices[AUX + 4 * index + 2] = index3;
				sphere_indices[AUX + 4 * index + 3] = index4;
			}
		}
	}
	*/


	auto procedural_sphere = [](f32 radius = 20.0f, u32 latBands = 32, u32 longBands = 32) {
		varray<f32> vertices;
		varray<u32> indices;

		for (u32 latNumber = 0; latNumber <= latBands; latNumber++) {
			f32 theta = latNumber * glm::pi<f32>() / latBands;
			f32 sinTheta = sin(theta);
			f32 cosTheta = cos(theta);

			for (int longNumber = 0; longNumber <= longBands; longNumber++) {
				f32 phi = longNumber * 2 * glm::pi<f32>() / longBands;
				f32 sinPhi = sin(phi);
				f32 cosPhi = cos(phi);

				f3 vertex(
					radius * cosPhi * sinTheta,
					radius * cosTheta,
					radius * sinPhi * sinTheta
				);
				vertices.insert(vertices.end(), { break3(vertex) });
			}
		}

		for (u32 latNumber = 0; latNumber < latBands; latNumber++) {
			for (u32 longNumber = 0; longNumber < longBands; longNumber++) {
				u32 first = (latNumber * (longBands + 1)) + longNumber;
				u32 second = first + longBands + 1;

				indices.push_back(first);
				indices.push_back(second);
				indices.push_back(first + 1);

				indices.push_back(second);
				indices.push_back(second + 1);
				indices.push_back(first + 1);
			}
		}

		return tuple_make(vertices, indices);
	};

	auto [sun_verts, sun_inds] = procedural_sphere(15.0f);
	auto [planet_verts, planet_inds] = procedural_sphere(8.0f);
	auto [satellite_verts, satellite_inds] = procedural_sphere(4.0f);


	auto procedural_cylinder = [](f32 radius = 10.0f, f32 height = 20.0f, u32 segments = 32) {
		varray<f32> vertices;
		varray<u32> indices;

		f32 halfHeight = height / 2.0f;

		for (u32 i = 0; i <= segments; i++) {
			f32 theta = glm::two_pi<f32>() * i / segments;
			f32 x = radius * glm::cos(theta);
			f32 z = radius * glm::sin(theta);

			vertices.insert(vertices.end(), { x, -halfHeight, z });
			vertices.insert(vertices.end(), { x, halfHeight, z });
		}

		for (u32 i = 0; i < segments; i++) {
			u32 bottom1 = 2 * i;
			u32 top1    = 2 * i + 1;
			u32 bottom2 = 2 * (i + 1);
			u32 top2    = 2 * (i + 1) + 1;

			indices.push_back(bottom1);
			indices.push_back(top1);
			indices.push_back(bottom2);

			indices.push_back(top1);
			indices.push_back(top2);
			indices.push_back(bottom2);
		}

		for (u32 i = 1; i < segments - 1; i++) {
			indices.push_back(0);
			indices.push_back(2 * i);
			indices.push_back(2 * (i + 1));
		}

		u32 topCenterIndex = 1;
		for (u32 i = 1; i < segments - 1; i++) {
			indices.push_back(topCenterIndex);
			indices.push_back(2 * i + 1);
			indices.push_back(2 * (i + 1) + 1);
		}

		return tuple_make(vertices, indices);
	};
	auto [tree_verts, tree_inds] = procedural_cylinder(5.0f, 50.0f);


	auto procedural_cone = [](f32 radius = 10.0f, f32 height = 15.0f, u32 segments = 32) {
		varray<f32> vertices;
		varray<u32> indices;

		f32 halfHeight = height / 2.0f;

		for (u32 i = 0; i < segments; i++) {
			f32 theta = glm::two_pi<f32>() * i / segments;
			f32 x = radius * glm::cos(theta);
			f32 z = radius * glm::sin(theta);

			vertices.insert(vertices.end(), { x, -halfHeight, z });
		}
		vertices.insert(vertices.end(), { 0.0f, halfHeight, 0.0f });  // top
		vertices.insert(vertices.end(), { 0.0f, -halfHeight, 0.0f }); // centre

		// Sides
		for (u32 i = 0; i < segments; i++) {
			u32 base1 = i;
			u32 base2 = (i + 1) % segments;
			u32 apex = segments;

			indices.push_back(base1);
			indices.push_back(base2);
			indices.push_back(apex);
		}

		// Circle
		for (u32 i = 0; i < segments; i++) {
			indices.push_back(i);
			indices.push_back((i + 1) % segments);
			indices.push_back(segments + 1);
		}

		return tuple_make(vertices, indices);
	};
	auto [leaves_verts, leaves_inds] = procedural_cone(20.0f, 50.0f);


	#pragma region Sun
	create_object("sun",
		std::move(sun_verts),
		none,
		none,
		std::move(sun_inds),
		none,
		none,
		"unlit.vert",
		"unlit.frag",
		none,
		none
	);
	#pragma endregion

	#pragma region Planet
	create_object("planet",
		std::move(planet_verts),
		none,
		none,
		std::move(planet_inds),
		none,
		none,
		"lit.vert",
		"lit.frag",
		none,
		none
	);
	#pragma endregion

	#pragma region Satellite
	create_object("satellite",
		std::move(satellite_verts),
		none,
		none,
		std::move(satellite_inds),
		none,
		none,
		"lit.vert",
		"lit.frag",
		none,
		none
	);
	#pragma endregion

	#pragma region Tree
	create_object("tree",
		std::move(tree_verts),
		none,
		none,
		std::move(tree_inds),
		none,
		none,
		"lit.vert",
		"lit.frag",
		none,
		none
	);
	#pragma endregion

	#pragma region Leaves
	create_object("leaves",
		[&]() { auto copy = leaves_verts; return copy; }(),
		none,
		none,
		[&]() { auto copy = leaves_inds; return copy; }(),
		none,
		none,
		"lit.vert",
		"lit.frag",
		none,
		none
	);
	#pragma endregion
}

void app::start() {
	app::camera.transform_delta(f3(0.0f, 0.0f, -200.0f), none);

	objects["sun"].transform_delta_mutable(f3(0.0f, 1.0f, 0.0f), none, none);
	objects["planet"].transform_delta_mutable(f3(50.0f, 0.0f, 0.0f), none, none);
	objects["satellite"].transform_delta_mutable(f3(70.0f, 0.0f, 0.0f), none, none);

	objects["tree"].transform_delta_mutable(f3(100.0f, 0.0f, 0.0f));

	make_instance("leaves", 2, none, {
		m4(1.0f) * glm::translate(m4(1.0f), f3(0.0f, 25.0f, 0.0f)) * glm::scale(m4(1.0f), f3(0.70f, 0.70f, 0.70f)),
		m4(1.0f) * glm::translate(m4(1.0f), f3(0.0f, 45.0f, 0.0f)) * glm::scale(m4(1.0f), f3(0.35f, 0.35f, 0.35f)),
	});
	objects["leaves"].transform_delta_mutable(f3(100.0f, 30.0f, 0.0f));
}


void app::update(f32 delta_time) {
	f3 move = f3(
		app::keys["d"] - app::keys["a"],
		app::keys["e"] - app::keys["q"],
		app::keys["w"] - app::keys["s"]
	);
	f3 rotate = f3(
		app::keys["up"] - app::keys["down"],
		app::keys["left"] - app::keys["right"],
		0.0f
	);
	move = normalized(move) * delta_time * 200.0f;
	rotate = normalized(rotate) * delta_time * 50.0f;
	app::camera.transform_delta(move, rotate);



	objects["sun"].set_uniform("color", f3(0.95f, 0.85f, 0.7f));

	objects["planet"].set_uniform("sun", objects["sun"].location);
	objects["planet"].set_uniform("color", f3(0.2f, 0.6f, 0.1f));
	objects["planet"].set_uniform("self", objects["planet"].location);

	objects["satellite"].set_uniform("sun", objects["sun"].location);
	objects["satellite"].set_uniform("color", f3(0.5f, 0.4f, 0.35f));
	objects["satellite"].set_uniform("self", objects["satellite"].location);

	auto [delta, _, __] = objects["planet"].transform_delta_mutable(none, f3(0.0, 20.0f * delta_time, 0.0f), none);
	objects["satellite"].set_origin(objects["planet"].location);
	objects["satellite"].transform_delta_mutable(delta, f3(0.0, 100.0f * delta_time, 0.0f), none);



	objects["tree"].set_uniform("sun", objects["sun"].location);
	objects["tree"].set_uniform("color", f3(0.4f, 0.2f, 0.1f));
	objects["tree"].set_uniform("self", objects["tree"].location);

	objects["leaves"].set_uniform("sun", objects["sun"].location);
	objects["leaves"].set_uniform("color", f3(0.4f, 0.7f, 0.2f));
	objects["leaves"].set_uniform("self", objects["leaves"].location);
}

///////////////////////////////////////////////////////////////////////////

#pragma region AppDefinitions
void app::redraw_start(Object &obj) {
	obj.visible = true;
	glDisable(GL_DEPTH_TEST);
}
template<class... Args> void app::redraw_start(Object &obj, Args&... args) {
	obj.visible = true;
	redraw_start(args...);
}


void app::redraw_end(Object &obj) {
	obj.visible = false;
	obj.transform(none, none, none);
	glEnable(GL_DEPTH_TEST);
}
template<class... Args> void app::redraw_end(Object &obj, Args&... args) {
	obj.visible = false;
	obj.transform(none, none, none);
	redraw_end(args...);
}
#pragma endregion
