#include "SaveManager.h"
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

void SaveManager::load(int id)
{
	const std::string dirPath = std::filesystem::current_path().string() + "\\saves\\" + std::to_string(id);
	const std::string filePath = dirPath + "\\game.data";

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary/* | std::ios::ate*/);
	if (!file.is_open()) {
		std::cout << "Failed to open file." << std::endl;
		return;
	}

	// process data
	file.read(&current.version, sizeof(char));

	switch (current.version)
	{
	case 1:
	default:
		// id
		file.read((char*)&current.id, sizeof(int));
		// size
		file.read((char*)&current.chunk_size, sizeof(short));

		// tiles
		/*const size_t size = (size_t)current.chunk_size * current.whuc * TILE_SIZE;
		char* tile_data = new char[size];
		current.tileData.clear();
		current.tileData.reserve(current.width * current.height);

		file.read(tile_data, size);

		for (size_t i = 0; i < current.width * current.height; i += TILE_SIZE)
		{
			current.tileData.push_back(
				Tile{
					(TileType)tile_data[i + 0]
				});
		}*/

		//delete[] tile_data;
		break;
	}

	file.close();
}

void SaveManager::save()
{
	const std::filesystem::path dir{ std::filesystem::current_path().string() + "\\saves\\" + std::to_string(current.id) };
	const std::string filePath = dir.string() + "\\game.data";

	if (!std::filesystem::exists(dir.string() + "\\chunks\\"))
		std::filesystem::create_directory(dir.string() + "\\chunks\\");

	std::ofstream file;
	file.open(filePath, std::ios::out | std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Failed to open file." << std::endl;
		return;
	}

	file.write(&current.version, sizeof(char));

	switch (SaveManager::current.version)
	{
	case 1:
	default:
		// id
		file.write((char*)&current.id, sizeof(int));
		// size
		file.write((char*)&current.chunk_size, sizeof(short));

		// tiles
		/*const size_t size = (size_t)current.chunk_size * current.chunk_size * TILE_SIZE;
		char* tile_data = new char[size];

		for (size_t i = 0; i < (size_t)current.chunk_size * current.chunk_size; i += TILE_SIZE)
		{
			Tile* tile = &SaveManager::current.tileData[i];
			tile_data[i + 0] = tile->type;
		}

		file.write(tile_data, size);*/

		break;
	}

	file.close();
}