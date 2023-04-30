#pragma once

#include <vector>
#include <chrono>
#include "tile.h"

struct Save {
	char version = 1;
	char* name;
	std::chrono::time_point<std::chrono::system_clock> created_time;
	std::chrono::time_point<std::chrono::system_clock> last_save_time;
	Tile* tileData;
};