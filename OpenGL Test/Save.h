#pragma once

#include <vector>
#include <chrono>
#include "tile.h"

struct Settings {
	float pan_x = 0, pan_y = 0;
};

struct Save {
	char version = 1;
	int id = -1;
	char* name;
	std::chrono::time_point<std::chrono::system_clock> created_time;
	std::chrono::time_point<std::chrono::system_clock> last_save_time;
	std::vector<Tile> tileData;
	unsigned width = 0, height = 0;
	Settings settings;
};