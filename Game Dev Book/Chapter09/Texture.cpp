#include "Texture.hpp"
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "SOIL/SOIL.h"

Texture::Texture() {
	mWidth = 0;
	mHeight = 0;
	mTextureID = 0;
}

Texture::~Texture() {

}

bool Texture::Load(const std::string& fileName) {
	int channels = 0;
	unsigned char* image = SOIL_load_image(
		fileName.c_str(),  // name of file
		&mWidth,  // stores width
		&mHeight,  // stores height
		&channels,  // stores number of channels
		SOIL_LOAD_AUTO  // type of image file, or auto for any
	);

	if (image == nullptr) {
		SDL_Log("SOIL failed to load image %s: %s", fileName.c_str(), SOIL_last_result());
		return false;
	}

	// check whether the image is RGB or RGBA
	int format = GL_RGB;
	if (channels == 4) {
		// RGBA instead
		format = GL_RGBA;
	}

	// create an OpenGL texture object and save the ID
	glGenTextures(1, &mTextureID);
	// set the texture as active
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	// copy raw image data into the texture object
	glTexImage2D(
		GL_TEXTURE_2D,  // texture target
		0,  // level of detail
		format,  // color format OpenGL should use
		mWidth,  // width of texture
		mHeight,  // height of texture
		0,  // border - this value must be 0
		format,  // color format of input data
		GL_UNSIGNED_BYTE,  // bit depth of input data, unsigned byte specifies 8-bit channels
		image  // pointer to image data
	);

	// tell SOIL to free the image from memory
	SOIL_free_image_data(image);

	// enable bilinear filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void Texture::Unload() {
	// delete texture object
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive() {
	glBindTexture(GL_TEXTURE_2D, mTextureID);
}