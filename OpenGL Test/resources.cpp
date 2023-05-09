#include <fstream>
#include <sstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "resources.hpp"


// Instantiate static variables
std::map<std::string, Shader> Resources::shaders;
std::map<std::string, Texture> Resources::textures;

std::string Resources::loadFromFile(path filePath)
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


Shader* Resources::loadShader(path vShaderPath, path fShaderPath, std::string name)
{
	shaders[name] = Shader{};
	shaders[name].compile(loadFromFile(vShaderPath).c_str(), loadFromFile(fShaderPath).c_str());
	return &shaders[name];
}


Shader* Resources::getShader(std::string name)
{
	return &shaders[name];
}


Texture Resources::loadTextureFromFile(path filePath, bool alpha, bool flip)
{
	Texture t;

	if (alpha)
	{
		t.image_format = GL_RGBA;
		t.internal_format = GL_RGBA;
	}

	stbi_set_flip_vertically_on_load(flip);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.string().c_str(), &width, &height, &nrChannels, 0);

	if (!data)
	{
		printf("Failed to load texture (%s)\n", filePath.string().c_str());
	}

	t.generate(width, height, data);

	stbi_image_free(data);

	return t;
}


void Resources::freeImageData(unsigned char* data)
{
	stbi_image_free(data);
}


GLFWimage Resources::textureFromFile(path filePath, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);

	GLFWimage image;

	image.pixels = stbi_load(filePath.string().c_str(), &image.width, &image.height, 0, 4);

	if (!image.pixels)
	{
		printf("Failed to load texture (%s)\n", filePath.string().c_str());
	}

	return image;
}


Texture* Resources::loadTexture(path filePath, bool alpha, std::string name, bool flip)
{
	return &(textures[name] = loadTextureFromFile(filePath, alpha, flip));
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

void Resources::saveScreenshotToFile(const char* fileName, int windowWidth, int windowHeight, int size, float scale)
{
	// TODO: https://stackoverflow.com/questions/25127751/opengl-read-pixels-faster-than-glreadpixels
	if (size > windowWidth) size = windowWidth;
	if (size > windowHeight) size = windowHeight;

	const int x = (windowWidth - size) / 2;
	const int y = (windowHeight - size) / 2;
	const int fS = size * scale;
	const int fD = (int)(1.0f / scale);
	const int numberOfPixels = fS * fS * 3;
	unsigned char* pixels = new unsigned char[numberOfPixels];

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_FRONT);

	long count = 0;
	for (int j = 0; j < size; j += fD)
	{
		for (int k = 0; k < size; k += fD)
		{
			glReadPixels(k + x, j + y, 1, 1, GL_BGR, GL_UNSIGNED_BYTE, pixels + count * 3);
			count++;
			//fwrite(pixel, 3, 1, outputFile);
		}
	}
	//glReadPixels(x, y, size, size, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	FILE* outputFile = fopen(fileName, "w");
	short header[] = { 0, 2, 0, 0, 0, 0, (short)fS, (short)fS, 24 };

	fwrite(&header, sizeof(header), 1, outputFile);
	fwrite(pixels, numberOfPixels, 1, outputFile);
	fclose(outputFile);

	delete[] pixels;

	printf("Finish writing to file.\n");
}