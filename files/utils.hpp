#pragma once

#include "datatypes.hpp"
#include <random>

struct Object;

static std::random_device rd;
static std::mt19937 mt(rd());

///////////////////////////////////////////////////////////////////////////

inline i32 jump_clamp_i32(i32 value, i32 min, i32 max) {
	if (value > max) {
		return min + (value - max - 1) % (max - min + 1);
	}
	else if (value < min) {
		return min + (min - value - 1) % (max - min + 1);
	}
	else {
		return value;
	}
}

template<class T> inline T jump_clamp(T value, T min, T max) {
	return
		(value >= min and value <= max) ? value
		: (value < min) ? max - (min - value)
		: min + (value - max);
}

template<class T> inline T jump_sin(T angle) {
	if (angle > glm::pi<T>() / 2.0 and angle < glm::pi<T>() * 3.0 / 2.0) {
		return -glm::sin(angle);
	}
	else {
		return glm::sin(angle);
	}
}

template<class T> inline T lerp(T min, T max, f32 t) {
	return min + t * (max - min);
}

void load_texture(Object &obj, str texture_path, bool has_opacity);

inline f32 random(f32 min, f32 max) {
	std::uniform_real_distribution<f32> dist(min, max);
	return dist(mt);
}
template<class T> inline T random(f32 min, f32 max);
template<> inline f1 random<f1>(f32 min, f32 max) {
	return f1(random(min, max));
}
template<> inline f2 random<f2>(f32 min, f32 max) {
	return f2(random(min, max), random(min, max));
}
template<> inline f3 random<f3>(f32 min, f32 max) {
	return f3(random(min, max), random(min, max), random(min, max));
}
template<> inline f4 random<f4>(f32 min, f32 max) {
	return f4(random(min, max), random(min, max), random(min, max), random(min, max));
}

template<class T> inline T vmap(T value, T old_min, T old_max, T new_min, T new_max) {
	return lerp(new_min, new_max, (value - old_min) / glm::abs(old_max - old_min));
}

inline f3 normalized(f3 vector) {
	f32 length = glm::length(vector);
	if (length > 0.0f) {
		return glm::normalize(vector);
	}
	else {
		return f3(0.0f);
	}
}

template<class T, u32 size> inline std::ostream& operator<<(std::ostream &out, const array<T, size> &v) {
	out << "{ ";
	for (u32 i = 0; i < v.size() - 1; i++) {
		out << v << ", ";
	}
	out << v[v.size() - 1] << " }";
	return out;
}

template<class T> inline std::ostream& operator<<(std::ostream &out, const varray<T> &v) {
	out << "{ ";
	for (u32 i = 0; i < v.size() - 1; i++) {
		out << v[i] << ", ";
	}
	out << v[v.size() - 1] << " }";
	return out;
}

template<class T> inline void print(str sep, T arg) {
	std::cout << arg << std::endl;
}
template<class T, class... Args> inline void print(str sep, T arg, Args... args) {
	std::cout << arg << sep;
	print(sep, args...);
}

#define n3(x, y, z) break3(normalized(f3(x, y, z)))
#define vassign1(v, value) *(&v) = value.x;
#define vassign2(v, value) \
{ \
	auto _value = value; \
	*(&v + 0) = _value.x; \
	*(&v + 1) = _value.y; \
}
#define vassign3(v, value) \
{ \
	auto _value = value; \
	*(&v + 0) = _value.x; \
	*(&v + 1) = _value.y; \
	*(&v + 2) = _value.z; \
}
#define vassign4(v, value) \
{ \
	auto _value = value; \
	*(&v + 0) = _value.x; \
	*(&v + 1) = _value.y; \
	*(&v + 2) = _value.z; \
	*(&v + 3) = _value.w; \
}
