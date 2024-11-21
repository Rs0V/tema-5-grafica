#version 460 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec4 in_Color;
layout (location = 2) in vec2 in_TexCoord;

out vec4 gl_Position;

out vec3 out_Position;
out vec4 out_Color;
out vec2 out_TexCoord;

uniform mat4 view = mat4(1.0);
uniform mat4 proj = mat4(1.0);
uniform mat4 myMatrix = mat4(1.0);

void main(void) {
	gl_Position = proj * view * myMatrix * in_Position;
	out_Position = in_Position.xyz;
	out_Color = in_Color;
    out_TexCoord = vec2(in_TexCoord.x, 1 - in_TexCoord.y);
}