#version 460 core

in vec3 out_Position;
in vec4 out_Color;
in vec3 out_Normal;
in vec2 out_TexCoord;

out vec4 FragColor;

uniform int useTex;
uniform sampler2D myTexture;

void main(void) {
	FragColor = (useTex == 1) ? texture(myTexture, out_TexCoord) : vec4(1.0f);
}