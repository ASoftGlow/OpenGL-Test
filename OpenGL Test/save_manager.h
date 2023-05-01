#pragma once
#include <vector>
#include <filesystem>

#include "save.h"

constexpr auto GAME_DATA_FILE = "world";
constexpr auto GAME_COMPANY = "ASoftGlow Software";
constexpr auto GAME_NAME = "Game";

using namespace std::filesystem;

class SaveManager {
public:
	static const char version = 1;
	/*
	* Saves current save to disk
	* @returns `true` if successfully saved, `false` if an error occurred
	*/
	static bool save();
	static bool save(int id);
	/*
	* Loads a save from disk
	* @param id - The unique id of the save
	* @returns `true` if successfully loaded, `false` if an error occurred
	*/
	static bool load(int id);
	static bool loadChunk(int x, int y);
	// TODO: error handling
	// TODO: make it so existing saves aren't overridden
	static void newSave(const char* name);
	static int duplicateSave(int id);
	static void renameSave(int id, char verison, const char* name);
	/*
	* Deletes the save with given id
	* @returns `true` if 1 or more files were removed
	*/
	static bool deleteSave(int id);
	/*
	* Deletes all save files
	* @returns `true` if 1 or more files were removed
	*/
	static bool deleteAllSaves();
	static std::vector<SavePreview> getSavesList();
	static path getSaveDir(int id) {
		return saves_dir / std::to_string(id);
	}
	static bool hasSavesDirChanged();
	static bool importSave(const char* path);
	static bool exportSave(const char* path);

	inline static Save current;
	inline static bool saved = false;

private:
	static size_t getChunkBlockSize();
	static int newId();
	static file_time_type getSavesDirWriteTime();

#ifndef _DEBUG
	inline static const path saves_dir{ std::string{ getenv("APPDATA") } + "\\" + GAME_COMPANY + "\\" + GAME_NAME + "\\saves" };
#else
	inline static const path saves_dir{ current_path() / "saves" };
#endif
	inline static file_time_type write_time = getSavesDirWriteTime();
};