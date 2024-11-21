#version 460 core

in vec3 out_Position;
in vec4 out_Color;
in vec3 out_Normal;
in vec2 out_TexCoord;

out vec4 FragColor;

uniform int useTex;
uniform sampler2D myTexture;

void main(void) {
	FragColor = out_Color;
}