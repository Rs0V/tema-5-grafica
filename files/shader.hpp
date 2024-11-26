#pragma once

#include "datatypes.hpp"

struct Object;

///////////////////////////////////////////////////////////////////////////

void create_shaders(Object &obj, maybe<str> vertex_shader_path, maybe<str> fragment_shader_path);
void destroy_shaders(Object &obj);
