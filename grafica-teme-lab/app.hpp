#pragma once

#include "camera.h"

struct Object;

///////////////////////////////////////////////////////////////////////////

namespace app {
	extern u32 fps;
	extern u32 frametime;


	extern umap<str, bool> keys;
	static void keyboard_down(u8 key, i32 x, i32 y) {
		str _key(1, key);
		keys[_key] = true;
	}
	static void keyboard_up(u8 key, i32 x, i32 y) {
		str _key(1, key);
		keys[_key] = false;
	}
	static void specialKey_down(i32 key, i32 x, i32 y) {
		switch (key) {
			case GLUT_KEY_LEFT:
				keys["left"] = true;
			break;

			case GLUT_KEY_RIGHT:
				keys["right"] = true;
			break;

			case GLUT_KEY_UP:
				keys["up"] = true;
			break;

			case GLUT_KEY_DOWN:
				keys["down"] = true;
			break;
		}
	}
	static void specialKey_up(i32 key, i32 x, i32 y) {
		switch (key) {
			case GLUT_KEY_LEFT:
				keys["left"] = false;
			break;

			case GLUT_KEY_RIGHT:
				keys["right"] = false;
			break;

			case GLUT_KEY_UP:
				keys["up"] = false;
			break;

			case GLUT_KEY_DOWN:
				keys["down"] = false;
			break;
		}
	}

	extern umap<str, bool> mouse_keys;
	static void mouse(i32 button, i32 state, i32 x, i32 y) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			mouse_keys["left"] = true;
		}
		else {
			mouse_keys["left"] = false;
		}

		if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			mouse_keys["right"] = true;
		}
		else {
			mouse_keys["right"] = false;
		}

		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
			mouse_keys["mid"] = true;
		}
		else {
			mouse_keys["mid"] = false;
		}
	}

	inline f32 get_time() {
		return glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	}

	void redraw_start(Object &obj);
	template<class... Args> void redraw_start(Object &obj, Args&... args);

	void redraw_end(Object &obj);
	template<class... Args> void redraw_end(Object &obj, Args&... args);



	extern Camera camera;
	void create();

	void start();

	void update(f32 delta_time);
}
