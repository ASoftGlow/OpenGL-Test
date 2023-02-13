#pragma once

#include <vector>

#include <glad/glad.h>


class ArrayTexture
{
public:
    unsigned ID;
    unsigned width, height;
    unsigned layerCount;
    unsigned internal_format; // format of texture object
    unsigned wrap_S;
    unsigned wrap_T;
    unsigned min_filter; // filtering mode if texture pixels < screen pixels
    unsigned max_filter; // filtering mode if texture pixels > screen pixels
    ArrayTexture();
    void generate(unsigned width, unsigned height, unsigned layerCount, unsigned char* data);
    void bind() const;
};