#version 460 core

in vec3 out_Position;
in vec4 out_Color;
in vec3 out_Normal;
in vec2 out_TexCoord;

//in vec3 out_FaceNormal;

out vec4 FragColor;

uniform int useTex;
uniform sampler2D myTexture;

uniform vec3 camera;
uniform vec3 light;

void main(void) {
	FragColor = (useTex == 1) ? texture(myTexture, out_TexCoord) : vec4(1.0f);
    
    vec3 normal = normalize(out_Normal);
    vec3 lightDir = normalize(light - out_Position);
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 result = diff * vec3(1.0, 1.0, 1.0);
    FragColor = FragColor * vec4(result, 1.0);
}
