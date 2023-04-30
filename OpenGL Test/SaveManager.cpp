#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

#include "SaveManager.h"

bool SaveManager::load(int id)
{
	const std::string dirPath = std::filesystem::current_path().string() + "\\saves\\" + std::to_string(id);
	const std::string filePath = dirPath + "\\game.data";

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary/* | std::ios::ate*/);
	if (!file.is_open()) {
		std::cerr << "Load - " "Failed to open file (" + filePath + ")" << std::endl;
		return false;
	}

	// process data
	file.read(&current.version, sizeof(char));

	switch (current.version)
	{
	case 1:
		// id
		file.read((char*)&current.id, sizeof(int));
		// chunk size
		file.read((char*)&current.chunk_size, sizeof(short));

		break;
	default:
		std::cerr << "Load - " "Unknown version" << std::endl;
		return false;
	}

	file.close();
	current.chunks.clear();

	return true;
}

bool SaveManager::loadChunk(int x, int y)
{
	const std::filesystem::path rootDir{ std::filesystem::current_path().string() + "\\saves\\" + std::to_string(current.id) + "\\chunks\\" };
	const std::string filePath = rootDir.string() + std::to_string(x) + "." + std::to_string(y) + ".data";

	if (!std::filesystem::exists(rootDir))
	{
		std::cerr << "Load - " "Directory doens't exist (" + rootDir.string() + ")" << std::endl;
		return false;
	}

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Load - " "Failed to open file (" + filePath + ")" << std::endl;
		return false;
	}

	char* tileData = new char[(unsigned)current.chunk_size * current.chunk_size];

	file.read((char*)tileData, sizeof(char) * (unsigned)current.chunk_size * current.chunk_size);
	current.chunks[std::pair<int, int>(x, y)] = Chunk{ x,y,&current.chunk_size };
	Chunk* chunk = &current.chunks.at(std::pair<int, int>(x, y));

	for (size_t i = 0; i < (size_t)current.chunk_size * current.chunk_size; i += TILE_SIZE)
	{
		if (tileData[i] < 0 || tileData[i] > 14)
		{
			std::cerr << "Load - " "Reading invalid data (" << (int)tileData[i] << ")" << std::endl;
			return false;
		}
		chunk->tiles[i] = Tile{
			(TileType)tileData[i]
		};
	}

	delete[] tileData;
	return true;
}

void SaveManager::newSave(int id)
{
	current.id = id;
	current.chunk_size = 16;
	current.chunks.clear();
	current.created_time = std::chrono::system_clock::now();
	current.last_save_time = std::chrono::system_clock::now();
}

bool SaveManager::deleteSave(int id)
{
	const std::filesystem::path rootDir{ std::filesystem::current_path().string() + "\\saves\\" + std::to_string(id) };
	return std::filesystem::remove_all(rootDir) > 0;
}

bool SaveManager::deleteAllSaves()
{
	const std::filesystem::path savesDir{ std::filesystem::current_path().string() + "\\saves" };
	uintmax_t removedCount = std::filesystem::remove_all(savesDir) > 0;
	std::filesystem::create_directory(savesDir);
	return removedCount > 0;
}

bool SaveManager::save()
{
	const std::filesystem::path rootDir{ std::filesystem::current_path().string() + "\\saves\\" + std::to_string(current.id) };
	const std::string chunksDir = rootDir.string() + "\\chunks\\";
	std::string filePath = rootDir.string() + "\\game.data";

	if (!std::filesystem::exists(rootDir))
		std::filesystem::create_directory(rootDir);
	if (!std::filesystem::exists(chunksDir))
		std::filesystem::create_directory(chunksDir);

	// Write game 
	std::ofstream file;
	file.open(filePath, std::ios::out | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Save - " "Failed to create file (" + filePath + ")" << std::endl;
		return false;
	}

	file.write(&current.version, sizeof(char));

	switch (SaveManager::current.version)
	{
	case 1:
		// id
		file.write((char*)&current.id, sizeof(int));
		// size
		file.write((char*)&current.chunk_size, sizeof(short));

		break;
	default:
		std::cerr << "Save - " "Unknown current version" << std::endl;
		return false;
	}

	file.close();

	// Write chunks
	for (auto& [pos, chunk] : current.chunks)
	{
		filePath = chunksDir + std::to_string(pos.first) + "." + std::to_string(pos.second) + ".data";

		file.open(filePath, std::ios::out | std::ios::binary);
		if (!file.is_open()) {
			std::cerr << "Save - " "Failed to create file (" + filePath + ")" << std::endl;
			return false;
		}

		switch (SaveManager::current.version)
		{
		case 1:
			for (Tile tile : chunk.tiles)
			{
				file.write(((char*)&tile.type), sizeof(char));
			}

			break;
		default:
			std::cerr << "Save - " "Unknown current version" << std::endl;
			return false;
		}

		file.close();
	}

	current.last_save_time = std::chrono::system_clock::now();

	return true;
}