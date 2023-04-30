#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>

#include "SaveManager.h"

size_t SaveManager::getChunkBlockSize()
{
	return (size_t)current.chunk_size * current.chunk_size * TILE_SIZE;;
}

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

void findChunk(std::ifstream* file, size_t chunk_block_size, int x, int y) {

	file->seekg(7, std::ios_base::beg);
	while (1)
	{
		if (!file->good()) break;
		int seekX, seekY;
		file->read((char*)&seekX, sizeof(int));
		file->read((char*)&seekY, sizeof(int));

		if (seekX == x && seekY == y) break;
		file->seekg(chunk_block_size, file->cur);
	}
}

bool SaveManager::loadChunk(int x, int y)
{
	printf("[DEBUG] Loading chunk (%i,%i)\n", x, y);

	const std::string dirPath = std::filesystem::current_path().string() + "\\saves\\" + std::to_string(current.id);
	const std::string filePath = dirPath + "\\game.data";

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Load - " "Failed to open file (" + filePath + ")" << std::endl;
		return false;
	}

	const size_t chunk_block_size = getChunkBlockSize();
	char* tileData;
	Chunk* chunk;
	switch (current.version)
	{
	case 1:
		// Find chunk
		findChunk(&file, chunk_block_size, x, y);
		if (file.tellg() == EOF) {
			std::cerr << "Load - " "Couldn't find chunk (" + std::to_string(x) + "," + std::to_string(y) + ")" << std::endl;
			return false;
		}

		tileData = new char[chunk_block_size];

		file.read((char*)tileData, chunk_block_size);
		current.chunks[std::pair<int, int>(x, y)] = Chunk{ x,y,&current.chunk_size };
		chunk = &current.chunks.at(std::pair<int, int>(x, y));

		for (size_t i = 0; i < (size_t)current.chunk_size * current.chunk_size; i += TILE_SIZE)
		{
			if (tileData[i] < 0 || tileData[i] > 48)
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

	default:
		std::cerr << "Save - " "Unknown current version" << std::endl;
		return false;
	}
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
	std::string filePath = rootDir.string() + "\\game.data";

	if (!std::filesystem::exists(rootDir))
		std::filesystem::create_directory(rootDir);

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

		// Write chunks
		for (auto& [pos, chunk] : current.chunks)
		{
			// x
			file.write((char*)&chunk.x, sizeof(int));
			// y
			file.write((char*)&chunk.y, sizeof(int));

			// tiles
			for (Tile tile : chunk.tiles)
			{
				file.write(((char*)&tile.type), TILE_SIZE);
			}
		}

		break;
	default:
		std::cerr << "Save - " "Unknown current version" << std::endl;
		return false;
	}

	file.close();

	current.last_save_time = std::chrono::system_clock::now();

	return true;
}

//bool SaveManager::saveChunk(int x, int y)
//{
//	const std::filesystem::path rootDir{ std::filesystem::current_path().string() + "\\saves\\" + std::to_string(current.id) };
//	std::string filePath = rootDir.string() + "\\game.data";
//
//	if (!std::filesystem::exists(filePath)) {
//		printf("Save - " "Game file doens't exist (%i)", current.id);
//		return false;
//	}
//
//	
//	// Write game 
//	std::ofstream file;
//	file.open(filePath, std::ios::out | std::ios::binary);
//	if (!file.is_open()) {
//		std::cerr << "Save - " "Failed to create file (" + filePath + ")" << std::endl;
//		return false;
//	}
//	
//	switch (SaveManager::current.version)
//	{
//	case 1:
//		findChunk(&file, getChunkBlockSize(), x, y);
//		
//		// x
//		file.write((char*)&chunk.x, sizeof(int));
//		// y
//		file.write((char*)&chunk.y, sizeof(int));
//
//		// tiles
//		for (Tile tile : chunk.tiles)
//		{
//			file.write(((char*)&tile.type), TILE_SIZE);
//		}
//
//		break;
//	default:
//		std::cerr << "Save - " "Unknown current version" << std::endl;
//		return false;
//	}
//
//	file.close();
//
//	current.last_save_time = std::chrono::system_clock::now();
//
//	return true;
//}