// learnopengl.com
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;

    void compile(const char* vertexShaderCode, const char* fragmentShaderCode);
    void use();

    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setUInt(const char* name, unsigned int value) const;
    void setFloat(const char* name, float value) const;
    void setVector2f(const char* name, float x, float y) const;
    void setMatrix4(const char* name, glm::mat4 mat) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};