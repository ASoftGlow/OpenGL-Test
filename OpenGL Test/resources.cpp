#include <fstream>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "resources.hpp"


// Instantiate static variables
std::map<std::string, Shader> Resources::shaders;
std::map<std::string, Texture> Resources::textures;

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


void Resources::clear()
{
	for (auto iter : shaders)
		glDeleteProgram(iter.second.ID);

	for (auto iter : textures)
		glDeleteTextures(1, &iter.second.ID);
}

void Resources::save(Terrain* terrain, const char* filePath)
{
	const unsigned size = sizeof(char) * 3 + sizeof(char) * (1) * terrain->width * terrain->height;
	// process data
	char* data = new char[size];

	// version
	data[0] = (char)1;
	// width
	data[1] = (char)terrain->width;
	// height
	data[2] = (char)terrain->height;
	// tile data
	for (unsigned i = 0; i < terrain->width * terrain->height; i++)
	{
		data[3 + i] = (char)terrain->getData()->at(i).type;
	}
	/*for (Tile t : *terrain->getData())
	{
		data += t.type;
	}*/
	//std::cout << data << std::endl;

	// save to file
	std::ofstream file;
	file.open(filePath, std::ios::out | std::ios::binary);
	file.write(data, size);
	file.close();

	delete[] data;
}

void Resources::load(Terrain* terrain, const char* filePath)
{
	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		std::cout << "Failed to open file." << std::endl;
		return;
	}
	std::streampos size = file.tellg();
	char* memblock = new char[size];
	file.seekg(0, std::ios::beg);
	file.read(memblock, size);
	file.close();

	// process data
	const char version = memblock[0];

	switch (version)
	{
	case 1:
	default:
		const unsigned width = (unsigned)memblock[1];
		const unsigned height = (unsigned)memblock[2];

		std::vector<Tile> tileData;
		tileData.reserve(width * height);

		for (unsigned int i = 0; i < width * height; i++)
		{
			tileData.push_back(Tile{ (TileType)memblock[3 + i] });
		}
		delete terrain;
		terrain = new Terrain{ width, height };
		terrain->loadData(tileData);

		break;
	}

	delete[] memblock;
}