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
	* @param[in] id The unique id of the save
	* @returns `true` if successfully loaded, `false` if an error occurred
	*/
	static bool load(int id);
	static bool loadChunk(int x, int y);
	// TODO: error handling
	static bool newSave();
	static bool deleteSave();
	static bool deleteAll();

	inline static Save current;
	inline static const char RELATIVE_PATH[] = "C:\\Users\\hankv\\source\\repos\\OpenGL Test\\OpenGL Test";
};