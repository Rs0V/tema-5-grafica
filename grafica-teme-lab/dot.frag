#version 460 core

in vec3 out_Position;
in vec4 out_Color;
in vec2 out_TexCoord;

out vec4 FragColor;

uniform int useTex;
uniform sampler2D myTexture;

uniform vec3 self;
uniform vec3 camera;

void main(void) {
	FragColor = (useTex == 1) ? texture(myTexture, out_TexCoord) : out_Color;
	FragColor = vec4(
		FragColor.rgb
		* clamp(dot(
			normalize(out_Position - self),
			normalize(camera - out_Position)
		), 0.3, 1.0),
		FragColor.a
	);
}