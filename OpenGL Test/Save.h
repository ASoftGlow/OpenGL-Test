#pragma once
#include <vector>
#include <map>
#include <chrono>
#include "chunk.h"

#define SAVE_NAME_SIZE 21

struct Save {
	char version = -1;
	int id = -1;
	char name[SAVE_NAME_SIZE];
	std::chrono::time_point<std::chrono::system_clock> created_time;
	std::chrono::time_point<std::chrono::system_clock> last_save_time;
	std::map<std::pair<int, int>, Chunk> chunks;
	unsigned short chunk_size = 0;
};

struct SavePreview {
	char version;
	int id;
	char name[SAVE_NAME_SIZE];
	std::chrono::time_point<std::chrono::system_clock> created_time;
	std::chrono::time_point<std::chrono::system_clock> last_save_time;
};