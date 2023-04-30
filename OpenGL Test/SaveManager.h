#pragma once
#include <vector>
#include <filesystem>

#include "Save.h"

#define saveRoot(id) saves_dir / std::to_string(id)
#define saveDataFile "game.data"

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
	static bool saveChunk(int x, int y);
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
		return path{ saveRoot(id) };
	}
	static bool hasSavesDirChanged();
	static int newId() {
		return rand();
	}

	inline static Save current;

private:
	static size_t getChunkBlockSize();

	inline static const path saves_dir{ current_path().string() + "\\saves" };
	inline static file_time_type write_time = last_write_time(saves_dir);
};