#pragma once

#include <string>
#include <map>

#include "shader_c.h"
#include "texture_c.h"


class Resources
{
public:
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture> textures;

	static Shader loadShader(const char* vertexShaderPath, const char* fragmentShaderPath, std::string name);
	static Shader getShader(std::string name);

	static Texture loadTexture(const char* filePath, bool alpha, std::string name);
	static Texture getTexture(std::string name);
	static void flipTextures(bool flip);

	static void clear();

private:
	static Texture loadTextureFromFile(const char* filePath, bool alpha);
	static std::string loadFromFile(const char* filePath);
};