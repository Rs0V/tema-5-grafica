#include "app.hpp"
#include "object.hpp"
#include "shader.hpp"

#define ENABLE_3D true

///////////////////////////////////////////////////////////////////////////

static m4 resizedMatrix;
void initialize(void) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


	app::create();

	#if ENABLE_3D == true
	m4 view = glm::lookAt(
		app::camera.location,
		app::camera.location + app::camera.forward,
		app::camera.up
	);
	m4 projection = glm::perspective(
		app::camera.fov,
		app::camera.width / app::camera.height,
		app::camera.znear,
		app::camera.zfar
	);
	for (auto &kv : objects) {
		auto &obj = kv.second;
		obj.set_uniform("view", view);
		obj.set_uniform("proj", projection);
	}
	#else
	resizedMatrix = glm::ortho(
		app::camera.xmin,
		app::camera.xmax,
		app::camera.ymin,
		app::camera.ymax
	);
	for (auto &kv : objects) {
		auto &obj = kv.second;
		obj.set_matrix(resizedMatrix);
	}
	#endif // ENABLE_3D == true

	app::start();
}

static i32 previous_time = 0;
void render_function(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.0f);

	i32 current_time = glutGet(GLUT_ELAPSED_TIME);
	f32 delta_time = (current_time - previous_time) / 1000.0f;
	previous_time = current_time;


	#if ENABLE_3D == true
	m4 view = glm::lookAt(
		app::camera.location,
		app::camera.location + app::camera.forward,
		app::camera.up
	);
	m4 projection = glm::perspective(
		app::camera.fov,
		app::camera.width / app::camera.height,
		app::camera.znear,
		app::camera.zfar
	);
	#endif // ENABLE_3D == true

	for (auto &kv : objects) {
		auto &obj = kv.second;

		#if ENABLE_3D == true
		obj.set_uniform("view", view);
		obj.set_uniform("proj", projection);
		#endif // ENABLE_3D == true

		obj.draw();
	}
	app::update(delta_time);


	glutSwapBuffers();
	glFlush();
}


void cleanup(void) {
	for (auto &kv : objects) {
		auto &obj = kv.second;
		destroy_shaders(obj);
		destroy_object(obj);
	}
}
void redraw(i32 ms) {
	glutPostRedisplay();
	glutTimerFunc(app::frametime, redraw, 0);
}


void resize_window(i32 width, i32 height) {
	glViewport(0, 0, width, height);
	app::camera.width  = glm::clamp((f32)width,  1.0f, 10000.0f);
	app::camera.height = glm::clamp((f32)height, 1.0f, 10000.0f);
}



i32 main(i32 argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize((i32)app::camera.width, (i32)app::camera.height);

	glutCreateWindow("OpenGL Window");

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(app::keyboard_down);
	glutKeyboardUpFunc(app::keyboard_up);
	glutSpecialFunc(app::specialKey_down);
	glutSpecialUpFunc(app::specialKey_up);
	glutMouseFunc(app::mouse);

	glutReshapeFunc(resize_window);

	glewInit();
	initialize();

	glutReshapeWindow((i32)app::camera.width, (i32)app::camera.height);

	glutDisplayFunc(render_function);
	glutTimerFunc(0, redraw, 0);
	glutCloseFunc(cleanup);

	glutMainLoop();
}
