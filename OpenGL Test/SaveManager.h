#pragma once

//#include "terrain.h"
#include "Save.h"

class SaveManager {
public:
	static void save();
	static void save(int id);
	static void load(int id);
	// TODO: error handling
	static void newSave();
	static void deleteSave();
	static void deleteAll();

	inline static Save current;
	inline static const char RELATIVE_PATH[] = "C:\\Users\\hankv\\source\\repos\\OpenGL Test\\OpenGL Test";
};