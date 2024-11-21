#include "utils.hpp"
#include "object.hpp"

///////////////////////////////////////////////////////////////////////////

void load_texture(Object &obj, str texture_path, bool has_opacity) {
	obj.use();

	glGenTextures(1, &obj.TEX);
	glBindTexture(GL_TEXTURE_2D, obj.TEX);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (has_opacity == false) {
		i32 width, height, channels;
		u8* image = SOIL_load_image(texture_path.data(), &width, &height, &channels, SOIL_LOAD_RGB);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		SOIL_free_image_data(image);
	}
	else {
		i32 width, height, channels;
		u8* image = SOIL_load_image(texture_path.data(), &width, &height, &channels, SOIL_LOAD_RGBA);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		SOIL_free_image_data(image);
	}
}
