#pragma once

#include "Save.h"

class SaveManager {
public:
	/* 
	* Saves current to disk
	* @returns `true` if successfully saved, `false` if an error occurred
	*/ 
	static bool save();
	/*
	* Loads a save from disk
	* @param id - The unique id of the save
	* @returns `true` if successfully loaded, `false` if an error occurred
	*/
	static bool load(int id);
	static bool loadChunk(int x, int y);
	// TODO: error handling

	static void newSave(int id);
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

	inline static Save current;
	inline static const char RELATIVE_PATH[] = "C:\\Users\\hankv\\source\\repos\\OpenGL Test\\OpenGL Test";
};