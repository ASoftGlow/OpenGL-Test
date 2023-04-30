#pragma once
#include <string>
#include <map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader_c.h"
#include "texture_c.h"
#include "terrain.h"


class Resources
{
public:
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture> textures;

	static Shader* loadShader(const char* vertexShaderPath, const char* fragmentShaderPath, std::string name);
	static Shader* getShader(std::string name);

	static Texture loadTexture(const char* filePath, bool alpha, std::string name, bool flip = true);
	static Texture* getTexture(std::string name);
	static GLFWimage textureFromFile(const char* file_path, bool flip = true);
	
	static void freeImageData(unsigned char* data);
	static void clear();

private:
	static Texture loadTextureFromFile(const char* filePath, bool alpha, bool flip = true);
	static std::string loadFromFile(const char* filePath);
};