#version 460 core

in vec3 out_Position;
in vec4 out_Color;
in vec3 out_Normal;
in vec2 out_TexCoord;

out vec4 FragColor;

uniform int useTex;
uniform sampler2D myTexture;

uniform vec3 sun;
uniform vec3 color;
uniform vec3 self;

float norm(float value) {
	return (value != 0.0) ? value : 0.0;
}
vec2 norm(vec2 value) {
	return (value.x != 0.0 || value.y != 0.0) ? normalize(value) : vec2(0.0);
}
vec3 norm(vec3 value) {
	return (value.x != 0.0 || value.y != 0.0 || value.z != 0.0) ? normalize(value) : vec3(0.0);
}
vec4 norm(vec4 value) {
	return (value.x != 0.0 || value.y != 0.0 || value.z != 0.0 || value.w != 0.0) ? normalize(value) : vec4(0.0);
}


void main(void) {
	FragColor = (useTex == 1) ? texture(myTexture, out_TexCoord) : vec4(1.0f);
	FragColor = vec4(color, 1.0) * mix(vec4(vec3(0.0f), 1.0f), FragColor, (dot(normalize(out_Position - self), normalize(sun - self)) + 1.0) / 2.0);
}