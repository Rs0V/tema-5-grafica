#include "shader.hpp"
#include "object.hpp"

///////////////////////////////////////////////////////////////////////////

void create_shaders(Object &obj, maybe<str> vertex_shader_path, maybe<str> fragment_shader_path) {
    defaults(vertex_shader_path, "default.vert");
    defaults(fragment_shader_path, "default.frag");

    obj.PROGRAM = LoadShaders(vertex_shader_path.value().data(), fragment_shader_path.value().data());
    glUseProgram(obj.PROGRAM);
}

void destroy_shaders(Object &obj) {
    glDeleteProgram(obj.PROGRAM);
}
