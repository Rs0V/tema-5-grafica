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
	#pragma region Pyramid
	create_object("pyramid",
		{
			-50.0f, -50.0f, -50.0f,
			 50.0f, -50.0f, -50.0f,
			 50.0f, -50.0f,  50.0f,
			-50.0f, -50.0f,  50.0f,
			  0.0f,  50.0f,   0.0f,
		},
		none,
		{
			n3(-1.0f, -1.0f, -1.0f),
			n3( 1.0f, -1.0f, -1.0f),
			n3( 1.0f, -1.0f,  1.0f),
			n3(-1.0f, -1.0f,  1.0f),
			n3( 0.0f,  1.0f,  0.0f),
		},
		{
			0, 1, 2,	0, 2, 3,
			0, 1, 4,
			1, 2, 4,
			2, 3, 4,
			3, 0, 4,
		},
		none,
		none,
		"random_color.vert",
		"random_color.frag",
		none,
		none
	);
	#pragma endregion

}

void app::start() {
	app::camera.transform_delta(f3(0.0f, 0.0f, -200.0f), none);
	//objects["pyramid"].set_uniform("light", objects["pyramid"].location + f3(200.0f, 100.0f, 100.0f));

	auto curve = [](f32 t) -> f3 {
		return f3(90 * t * glm::sin(t), 90 * t, 90 * t * glm::cos(t));
	};
	varray<m4> inst(20);
	varray<f32> cols(inst.size() * 4);
	for (u32 i = 0; i < inst.size(); i++) {
		inst[i] = glm::translate(m4(1.0f), curve((i + 1) / 2.0f));
		vassign4(cols[i * 4], f4(random<f3>(0.0f, 1.0f), 1.0f));
	}
	make_instance("pyramid", inst.size(), std::move(cols), std::move(inst));

	//print("", objects["pyramid"].colors);
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

	//objects["pyramid"].set_uniform("camera", app::camera.location);

	//print("\t\t", app::camera.location, app::camera.rotation);
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
