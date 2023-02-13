#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "resources.hpp"


// Instantiate static variables
std::map<std::string, Shader> Resources::shaders;
std::map<std::string, Texture> Resources::textures;
std::map<std::string, ArrayTexture> Resources::arrayTextures;

std::string Resources::loadFromFile(const char* filePath)
{
	std::string code;
	std::ifstream file;
	// ensure ifstream objects can throw exceptions:
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		file.open(filePath);
		std::stringstream fileStream;

		// read file's buffer contents into streams
		fileStream << file.rdbuf();

		file.close();

		code = fileStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
	}
	return code;
}


Shader Resources::loadShader(const char* vShaderPath, const char* fShaderPath, std::string name)
{
	Shader s;
	s.compile(loadFromFile(vShaderPath).c_str(), loadFromFile(fShaderPath).c_str());
	shaders[name] = s;
	return s;
}


Shader* Resources::getShader(std::string name)
{
	return &shaders[name];
}


Texture Resources::loadTextureFromFile(const char* filePath, bool alpha, bool flip)
{
	Texture t;

	if (alpha)
	{
		t.image_format = GL_RGBA;
		t.internal_format = GL_RGBA;
	}

	stbi_set_flip_vertically_on_load(flip);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
	//std::cout << filePath << std::endl;
	//std::cout << data << std::endl;

	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	t.generate(width, height, data);

	stbi_image_free(data);

	return t;
}


void Resources::freeImageData(unsigned char* data)
{
	stbi_image_free(data);
}


GLFWimage Resources::textureFromFile(const char* file_path, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);

	GLFWimage image;

	image.pixels = stbi_load(file_path, &image.width, &image.height, 0, 4);

	if (!image.pixels)
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	return image;
}


Texture Resources::loadTexture(const char* filePath, bool alpha, std::string name, bool flip)
{
	Texture t = loadTextureFromFile(filePath, alpha, flip);
	textures[name] = t;
	return t;
}


Texture* Resources::getTexture(std::string name)
{
	return &textures[name];
}

ArrayTexture Resources::loadArrayTexture(const char* dirPath, std::string name, bool flip)
{
	ArrayTexture t;
	int width = 0, height = 0, cha;
	std::vector<unsigned char> images;
	char tileCount = 0;

	stbi_set_flip_vertically_on_load(flip);

	t.internal_format = GL_RGBA;

	// get every image file
	for (const auto& entry : std::filesystem::directory_iterator(dirPath))
	{
		if (!entry.is_regular_file()) continue;
		if (!(entry.path().extension().string() == ".png")) continue;
		
		unsigned char* data = stbi_load(entry.path().string().data(), &width, &height, &cha, 0);
		if (!data) std::cerr << "Failed to load texture" << std::endl;

		const int size = width * height * 4;
		images.insert(images.end(), data, data + size);
		tileCount += 1;
		free(data);
	}
	
	t.generate(width, height, tileCount, images.data());


	arrayTextures[name] = t;
	return t;
}

ArrayTexture* Resources::getArrayTexture(std::string name)
{
	return &arrayTextures[name];
}


void Resources::clear()
{
	for (auto iter : shaders)
		glDeleteProgram(iter.second.ID);

	for (auto iter : textures)
		glDeleteTextures(1, &iter.second.ID);
}