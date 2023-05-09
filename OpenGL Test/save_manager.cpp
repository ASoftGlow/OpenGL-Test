#include <fstream>
#include <string>
#include <iostream>
#include <limits>
#include <random>
#include "libzippp/libzippp.h"

#include "save_manager.h"
#include "logger.h"
#include "resources.hpp"
#include "platform_paths.h"
#include "config.h"

using namespace std::filesystem;
using namespace libzippp;

const int CHUNKS_START_OFFSET = sizeof(char) + sizeof(char) * SAVE_NAME_SIZE + sizeof(time_t) * 2 + sizeof(short);
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dis(0, INT_MAX);

int SaveManager::newId()
{
	return dis(gen);
}

size_t SaveManager::getChunkBlockSize()
{
	return (size_t)current.chunk_size * current.chunk_size * TILE_SIZE;;
}

file_time_type SaveManager::getSavesDirWriteTime()
{
	if (!std::filesystem::exists(platform_paths::getSavesFolder()))
		return file_time_type(std::chrono::system_clock::duration(0));
	return last_write_time(platform_paths::getSavesFolder());
}

bool SaveManager::hasSavesDirChanged()
{
	const auto new_write_time = getSavesDirWriteTime();
	if ((new_write_time - write_time).count() != 0)
	{
		write_time = new_write_time;
		return true;
	}
	return false;
}

bool SaveManager::load(int id)
{
	if (id < 0) return false;

	const path filePath = platform_paths::getSaveFolder(id) / GAME::SAVE::WORLD_FILE;

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary);
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

	saved = true;
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

	const path filePath = platform_paths::getSaveFolder(current.id) / GAME::SAVE::WORLD_FILE;

	std::ifstream file;
	file.open(filePath, std::ios::in | std::ios::binary);
	if (!file.is_open()) {
		//std::cerr << "Load - " "Failed to open file (" << filePath << ")" << std::endl;
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
		current.chunks[std::pair<int, int>(x, y)] = Chunk(x, y, current.chunk_size);
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
	saved = false;
	current.version = version;
	current.id = newId();
	current.chunk_size = 16;
	current.chunks.clear();
	current.created_time = std::chrono::system_clock::now();
	current.last_save_time = std::chrono::system_clock::from_time_t(1);
	strcpy(current.name, name);
}

int SaveManager::duplicateSave(int sourceId)
{
	const int destId = newId();
	const path sourceDir{ platform_paths::getSaveFolder(sourceId) };
	const path destDir{ platform_paths::getSaveFolder(destId) };

	copy(sourceDir, destDir, std::filesystem::copy_options::recursive);

	return destId;
}

void SaveManager::renameSave(int id, char version, const char* name)
{
	const path filePath = platform_paths::getSaveFolder(id) / GAME::SAVE::WORLD_FILE;

	// Write game 
	std::fstream file;
	file.open(filePath, std::ios::in | std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		std::cerr << "Rename - " "Failed to open file (" << filePath << ")" << std::endl;
		return;
	}
	const std::fpos_t size = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buff = new char[size];
	file.read(buff, size);
	file.close();

	switch (version)
	{
	case 1:
		char* name_pos = buff + sizeof(char);
		strcpy(name_pos, name);
	}

	file.open(filePath, std::ios::out | std::ios::binary);
	file.write(buff, size);
	delete[] buff;
	file.close();
}

bool SaveManager::deleteSave(int id)
{
	if (!exists(platform_paths::getSavesFolder())) return false;
	if (id < 0) return false;
	const path rootDir{ platform_paths::getSaveFolder(id) };
	return remove_all(rootDir) > 0;
}

bool SaveManager::deleteAllSaves()
{
	if (!exists(platform_paths::getSavesFolder())) return 0;
	uintmax_t removedCount = remove_all(platform_paths::getSavesFolder()) > 0;
	create_directory(platform_paths::getSavesFolder());
	return removedCount > 0;
}

std::vector<SavePreview> SaveManager::getSavesList()
{
	std::vector<SavePreview> saves = std::vector<SavePreview>();

	if (exists(platform_paths::getSavesFolder()))
		for (const directory_entry& entry : directory_iterator(platform_paths::getSavesFolder(), directory_options::skip_permission_denied))
		{
			std::ifstream file;
			file.open(entry.path() / GAME::SAVE::WORLD_FILE, std::ios::in | std::ios::binary);
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


bool SaveManager::save()
{
	return save(current.id);
}

bool SaveManager::save(int id)
{
	if (id < 0) return false;

	current.last_save_time = std::chrono::system_clock::now();

	const path rootDir{ platform_paths::getSaveFolder(id) };
	const path filePath = rootDir / GAME::SAVE::WORLD_FILE;

	if (!exists(platform_paths::getSavesFolder()))
		create_directory(platform_paths::getSavesFolder());
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

	// Save thumbnail
	//Resources::saveScreenshotToFile(rootDir / GAME_SAVE_THUMBNAIL_FILE, 600, 600, 400, 0.04f);

	saved = true;
	return true;
}

bool SaveManager::exportSave(const char* path)
{
	ZipArchive zf{ path };
	if (!zf.open(ZipArchive::New))
		return false;
	zf.setComment("Hello there :)");
	zf.addFile(GAME::SAVE::WORLD_FILE, (platform_paths::getSaveFolder(current.id) / GAME::SAVE::WORLD_FILE).string());

	zf.close();
	return true;
}

bool SaveManager::importSave(const char* file_path)
{
	int count = 0;
	path rootDir;
	int id;
	do {
		count++;
		id = newId();
		rootDir = platform_paths::getSaveFolder(id);
	} while (exists(rootDir) && count != INT_MAX);
	if (count == INT_MAX)
	{
		Logger::error("Max num saves reached\nHOW?!?");
		return false;
	}

	if (!exists(platform_paths::getSavesFolder()))
		create_directory(platform_paths::getSavesFolder());
	create_directory(rootDir);

	ZipArchive zf{ file_path };
	if (!zf.open(ZipArchive::ReadOnly))
		return false;

	std::vector<ZipEntry> entries = zf.getEntries();
	if (entries.size() == 0)
		return false;

	std::vector<ZipEntry>::iterator it;
	for (it = entries.begin(); it != entries.end(); ++it) {
		ZipEntry entry = *it;
		if (entry.isFile())
		{
			const path output_path = rootDir / entry.getName();
			std::ofstream file(output_path, std::ios::out | std::ios::binary);
			const int result = entry.readContent(file);
			file.close();

			if (result != LIBZIPPP_OK)
			{
				Logger::error("Load - " "Failed to import");
				deleteSave(id);
				zf.close();
				return false;
			}
		}
	}

	zf.close();
	return true;
}
