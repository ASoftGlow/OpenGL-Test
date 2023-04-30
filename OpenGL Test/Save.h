#pragma once

#include <vector>
#include <map>
#include <chrono>
#include "Chunk.h"

struct Settings {
	float pan_x = 0, pan_y = 0;
};

struct Save {
	char version = 1;
	int id = -1;
	char* name;
	std::chrono::time_point<std::chrono::system_clock> created_time;
	std::chrono::time_point<std::chrono::system_clock> last_save_time;
	std::map<std::pair<int, int>, Chunk> chunks;
	unsigned short chunk_size = 0;
	Settings settings;
};