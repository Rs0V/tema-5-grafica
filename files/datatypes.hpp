#pragma once

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "loadShaders.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <string>

#include "SOIL.h"

#include <optional>
#include <tuple>

///////////////////////////////////////////////////////////////////////////

using cstr = const char*;
using str = std::string;

using boolean = GLboolean;

using u8  = GLubyte;
using u16 = GLushort;
using u32 = GLuint;
using u64 = GLuint64;

using i8  = GLbyte;
using i16 = GLshort;
using i32 = GLint;
using i64 = GLint64;

using f16 = GLhalf;
using f32 = GLfloat;
using f64 = GLdouble;

using option = GLenum;

using f1 = glm::vec1;
using f2 = glm::vec2;
using f3 = glm::vec3;
using f4 = glm::vec4;

using d1 = glm::dvec1;
using d2 = glm::dvec2;
using d3 = glm::dvec3;
using d4 = glm::dvec4;

using i1 = glm::ivec1;
using i2 = glm::ivec2;
using i3 = glm::ivec3;
using i4 = glm::ivec4;

using u1 = glm::uvec1;
using u2 = glm::uvec2;
using u3 = glm::uvec3;
using u4 = glm::uvec4;

using b1 = glm::bvec1;
using b2 = glm::bvec2;
using b3 = glm::bvec3;
using b4 = glm::bvec4;

#define break1(vec) vec.x
#define break2(vec) vec.x, vec.y
#define break3(vec) vec.x, vec.y, vec.z
#define break4(vec) vec.x, vec.y, vec.z, vec.w

using m4 = glm::mat4;
#define m4_make glm::make_mat4

using m3 = glm::mat3;
#define m3_make glm::make_mat3

using q4 = glm::quat;
#define q4_make glm::angleAxis

#define null nullptr

#define umap std::unordered_map
#define map std::map

#define uset std::unordered_set
#define set std::set

#define array std::array
#define varray std::vector


template<typename T, std::size_t N, typename... Ts>
struct repeat_type {
	using type = typename repeat_type<T, N - 1, T, Ts...>::type;
};

template<typename T, typename... Ts>
struct repeat_type<T, 0, Ts...> {
	using type = std::tuple<Ts...>;
};

#define tuple std::tuple
#define tarray(_type, _count) repeat_type<_type, _count>::type
#define tuple_make std::make_tuple


#define maybe std::optional
#define none {}
#define defaults(var, value) if (!var) { var = value; }
#define vdefaults(_varray, ...) if (_varray.empty() == true) { _varray = {__VA_ARGS__}; }
#define vfordef(_varray, repeats, ...) \
{ \
	_varray = {__VA_ARGS__}; \
	u32 size = _varray.size(); \
	u32 rep = repeats; \
	_varray.reserve(size * rep); \
	for (u32 i = 0; i < rep - 1; i++) { \
		_varray.insert(_varray.end(), _varray.begin(), _varray.begin() + size); \
	} \
}

#define print_vec1(type) \
inline std::ostream& operator<<(std::ostream &out, const type &vec) { \
	out << vec.x; \
	return out; \
}
#define print_vec2(type) \
inline std::ostream& operator<<(std::ostream &out, const type &vec) { \
	out << vec.x << " " << vec.y; \
	return out; \
}
#define print_vec3(type) \
inline std::ostream& operator<<(std::ostream &out, const type &vec) { \
	out << vec.x << " " << vec.y << " " << vec.z; \
	return out; \
}
#define print_vec4(type) \
inline std::ostream& operator<<(std::ostream &out, const type &vec) { \
	out << vec.x << " " << vec.y << " " << vec.z << " " << vec.w; \
	return out; \
}

print_vec1(u1);
print_vec1(i1);
print_vec1(f1);
print_vec1(d1);
print_vec1(b1);

print_vec2(u2);
print_vec2(i2);
print_vec2(f2);
print_vec2(d2);
print_vec2(b2);

print_vec3(u3);
print_vec3(i3);
print_vec3(f3);
print_vec3(d3);
print_vec3(b3);

print_vec4(u4);
print_vec4(i4);
print_vec4(f4);
print_vec4(d4);
print_vec4(b4);
