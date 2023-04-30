#pragma once

//#include "terrain.h"
#include "Save.h"

class SaveManager {
public:
	SaveManager();

	int save();
	void save(unsigned id);
	void load(unsigned id);
	// TODO: error handling
	void newSave();
	void deleteSave();
	void deleteAll();

	Save current_save;
};