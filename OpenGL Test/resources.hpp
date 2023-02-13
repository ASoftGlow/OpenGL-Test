#pragma once

#include <string>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader_c.h"
#include "texture_c.h"
#include "ArrayTexture.h"


class Resources
{
public:
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture> textures;
	static std::map<std::string, ArrayTexture> arrayTextures;

	static Shader loadShader(const char* vertexShaderPath, const char* fragmentShaderPath, std::string name);
	static Shader* getShader(std::string name);

	static Texture loadTexture(const char* filePath, bool alpha, std::string name, bool flip = true);
	static Texture* getTexture(std::string name);
	static ArrayTexture loadArrayTexture(const char* filePath, std::string name, bool flip = true);
	static ArrayTexture* getArrayTexture(std::string name);
	//static ArrayTexture loadArrayTextureFromDir(const char* dirPath, bool flip);
	static GLFWimage textureFromFile(const char* file_path, bool flip = true);
	static void freeImageData(unsigned char* data);

	static void clear();

private:
	static Texture loadTextureFromFile(const char* filePath, bool alpha, bool flip = true);
	static std::string loadFromFile(const char* filePath);
};