#pragma once
#include <string>
#include <map>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>

#include "shader_c.h"
#include "texture_c.h"
#include "terrain.h"

using namespace std::filesystem;

class Resources
{
public:
	static std::map<std::string, Shader> shaders;
	static std::map<std::string, Texture> textures;

	static Shader* loadShader(path vertexShaderPath, path fragmentShaderPath, std::string name);
	static Shader* getShader(std::string name);

	static Texture* loadTexture(path filePath, bool alpha, std::string name, bool flip = true);
	static Texture* getTexture(std::string name);
	static GLFWimage textureFromFile(path filePath, bool flip = true);

	static void freeImageData(unsigned char* data);
	static void clear();

	static void saveScreenshotToFile(std::filesystem::path filePath, int windowWidth, int windowHeight, int size, float scale)
	{
		saveScreenshotToFile(filePath.string().c_str(), windowWidth, windowHeight, size, scale);
	};
	static void saveScreenshotToFile(const char* fileName, int windowWidth, int windowHeight, int size, float scale);

private:
	static Texture loadTextureFromFile(path filePath, bool alpha, bool flip = true);
	static std::string loadFromFile(path filePath);
};