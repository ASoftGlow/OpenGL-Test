#include <fstream>
#include <string>
#include <iostream>

#include "SaveManager.h"

using namespace std::filesystem;

const int CHUNKS_START_OFFSET = sizeof(char) + sizeof(char) * 21 + sizeof(time_t) * 2 + sizeof(short);


size_t SaveManager::getChunkBlockSize()
{
	return (size_t)current.chunk_size * current.chunk_size * TILE_SIZE;;
}

bool SaveManager::load(int id)
{
	if (id < 0) return false;

	const path filePath = saveRoot(id) / saveDataFile;

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary/* | std::ios::ate*/);
	if (!file.is_open()) {
		std::cerr << "Load - " "Failed to open file (" << filePath << ")" << std::endl;
		return false;
	}

	// process data
	file.read(&current.version, sizeof(char));

	switch (current.version)
	{
	case 1:
		current.id = id;
		// name
		file.read(current.name, sizeof(current.name));
		// created time
		time_t created_time;
		file.read((char*)&created_time, sizeof(created_time));
		current.created_time = std::chrono::system_clock::from_time_t(created_time);
		// saved time
		time_t saved_time;
		file.read((char*)&saved_time, sizeof(saved_time));
		current.last_save_time = std::chrono::system_clock::from_time_t(saved_time);
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

	file->seekg(CHUNKS_START_OFFSET, std::ios_base::beg);
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

	const path filePath = saveRoot(current.id) / saveDataFile;

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Load - " "Failed to open file (" << filePath << ")" << std::endl;
		return false;
	}

	const size_t chunk_block_size = getChunkBlockSize();

	switch (current.version)
	{
	case 1:
	{
		// Find chunk
		findChunk(&file, chunk_block_size, x, y);
		if (file.tellg() == EOF) {
			std::cerr << "Load - " "Couldn't find chunk (" + std::to_string(x) + "," + std::to_string(y) + ")" << std::endl;
			return false;
		}

		char* tileData = new char[chunk_block_size];

		file.read((char*)tileData, chunk_block_size);
		current.chunks[std::pair<int, int>(x, y)] = Chunk{ x,y,&current.chunk_size };
		Chunk* chunk = &current.chunks.at(std::pair<int, int>(x, y));

		for (size_t i = 0; i < chunk_block_size; i += TILE_SIZE)
		{
			if (tileData[i] < Grass || tileData[i] > MISSING)
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
	default:
		std::cerr << "Save - " "Unknown current version" << std::endl;
		return false;
	}
}

void SaveManager::newSave(const char* name)
{
	current.version = version;
	current.id = newId();
	current.chunk_size = 16;
	current.chunks.clear();
	current.created_time = std::chrono::system_clock::now();
	current.last_save_time = std::chrono::system_clock::now();
	strcpy(current.name, name);
}

bool SaveManager::deleteSave(int id)
{
	if (id < 0) return false;
	const path rootDir{ saveRoot(id) };
	return remove_all(rootDir) > 0;
}

bool SaveManager::deleteAllSaves()
{
	uintmax_t removedCount = remove_all(saves_dir) > 0;
	create_directory(saves_dir);
	return removedCount > 0;
}

std::vector<SavePreview> SaveManager::getSavesList()
{
	std::vector<SavePreview> saves = std::vector<SavePreview>();

	for (const directory_entry& entry : directory_iterator(saves_dir, directory_options::skip_permission_denied))
	{
		std::ifstream file;
		file.open(entry.path() / saveDataFile, std::ios::in | std::ios::binary);
		if (!file.is_open())
			continue;

		SavePreview save;

		// process data
		file.read(&save.version, sizeof(char));

		switch (save.version)
		{
		case 1:
			// id
			save.id = std::stoi(entry.path().filename().string());
			if (save.id < 0) continue;
			// name
			file.read(save.name, sizeof(save.name));
			// created time
			time_t created_time;
			file.read((char*)&created_time, sizeof(created_time));
			save.created_time = std::chrono::system_clock::from_time_t(created_time);
			// saved time
			time_t saved_time;
			file.read((char*)&saved_time, sizeof(saved_time));
			save.last_save_time = std::chrono::system_clock::from_time_t(saved_time);
			break;

		default:
			continue;
		}

		saves.push_back(save);
	}
	return saves;
}

bool SaveManager::hasSavesDirChanged()
{
	const auto new_write_time = last_write_time(saves_dir);
	if ((new_write_time - write_time).count() != 0)
	{
		write_time = new_write_time;
		return true;
	}
	return false;
}


bool SaveManager::save()
{
	return save(current.id);
}

bool SaveManager::save(int id)
{
	if (id < 0) return false;

	current.last_save_time = std::chrono::system_clock::now();

	const path rootDir{ saveRoot(id) };
	const path filePath = rootDir / saveDataFile;

	if (!exists(rootDir))
		create_directory(rootDir);

	// Write game 
	std::ofstream file;
	file.open(filePath, std::ios::out | std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Save - " "Failed to create file (" << filePath << ")" << std::endl;
		return false;
	}

	file.write(&current.version, sizeof(char));

	switch (SaveManager::current.version)
	{
	case 1:
	{
		// name
		file.write(current.name, sizeof(current.name));
		// created time
		const time_t created_time = std::chrono::system_clock::to_time_t(current.created_time);
		file.write((char*)&created_time, sizeof(created_time));
		// saved time
		const time_t saved_time = std::chrono::system_clock::to_time_t(current.last_save_time);
		file.write((char*)&saved_time, sizeof(saved_time));
		// chunk size
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
	}
	default:
		std::cerr << "Save - " "Unknown current version" << std::endl;
		return false;
	}

	file.close();


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