#include "ArrayTexture.h"
#include <iostream>

ArrayTexture::ArrayTexture()
	: width(0), height(0), layerCount(1), internal_format(GL_RGB),
	wrap_S(GL_CLAMP_TO_EDGE), wrap_T(GL_CLAMP_TO_EDGE), min_filter(GL_NEAREST), max_filter(GL_NEAREST)
{
	glGenTextures(1, &ID);
}


void ArrayTexture::generate(unsigned width, unsigned height, unsigned layerCount, unsigned char* data)
{
	this->width = width;
	this->height = height;
	this->layerCount = layerCount;

	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
	// Allocate the storage.
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, this->width, this->height, this->layerCount);

	// Upload pixel data.
	// The first 0 refers to the mipmap level (level 0, since there's only 1)
	// The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
	// The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
	// Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, this->width, this->height, this->layerCount, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap_S);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap_T);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, max_filter);

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}


void ArrayTexture::bind() const
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
}