#include "texture_c.h"


Texture::Texture() 
	: width(0), height(0), internal_format(GL_RGB), image_format(GL_RGB), 
	wrap_S(GL_CLAMP_TO_EDGE), wrap_T(GL_CLAMP_TO_EDGE),min_filter(GL_NEAREST), max_filter(GL_NEAREST)
{
	glGenTextures(1, &ID);
}


void Texture::generate(unsigned int width, unsigned int height, unsigned char* data)
{
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, max_filter);

	glBindTexture(GL_TEXTURE_2D, 0);	
}


void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, ID);
}