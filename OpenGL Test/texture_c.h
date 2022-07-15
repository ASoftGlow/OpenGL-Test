// learnopengl.com
#pragma once

#include <glad/glad.h>


class Texture
{
public:
    unsigned int ID;
    unsigned int width, height;
    unsigned int internal_format; // format of texture object
    unsigned int image_format; // format of loaded image
    unsigned int wrap_S;
    unsigned int wrap_T;
    unsigned int min_filter; // filtering mode if texture pixels < screen pixels
    unsigned int max_filter; // filtering mode if texture pixels > screen pixels
    Texture();
    void generate(unsigned int width, unsigned int height, unsigned char* data);
    void bind() const;
};