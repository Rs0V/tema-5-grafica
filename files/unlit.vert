#version 460 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec4 in_Color;
layout (location = 2) in vec3 in_Normal;
layout (location = 3) in vec2 in_TexCoord;
layout (location = 4) in mat4 in_Instance;

out vec4 gl_Position;

out vec3 out_Position;
out vec4 out_Color;
out vec3 out_Normal;
out vec2 out_TexCoord;

uniform mat4 view = mat4(1.0);
uniform mat4 proj = mat4(1.0);
uniform mat4 myMatrix = mat4(1.0);

void main(void) {
	gl_Position = proj * view * myMatrix * in_Instance * vec4(in_Position, 1.0);

	out_Position = vec3(myMatrix * in_Instance * vec4(in_Position, 1.0));
	out_Color = in_Color;
	out_Normal = in_Normal;
    out_TexCoord = vec2(in_TexCoord.x, 1 - in_TexCoord.y);
}