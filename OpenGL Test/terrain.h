#pragma once
#include <vector>
#include <map>
#include <iomanip>
#include <random>

#include "chunk.h"

class Terrain {
public:
	Terrain(unsigned short* pSize, std::map<std::pair<int, int>, Chunk>* chunks_ptr);
	Chunk* getChunk(int x, int y);
	// @param blend Uses existing chunks to blend borders
	void generateChunk(int chunk_x, int chunk_y, bool blend = true);
	void generateChunks(std::vector<std::pair<int, int>> chunks);
	void init();

	unsigned short* chunk_size;
	std::map<std::pair<int, int>, Chunk>* chunks;

	static constexpr char tile_rules[][4] = {
		/*
		0: full grass
		1: full sea
		2,3: grass/sea
		4: full sand
		5,6: sand/sea
		7: cactas
		8,9: sand/grass
		*/
		// 0,1,2,3
	  // S,E,N,W
		{0,0,0,0}, // grass
		{3,0,0,3}, // grass L corner
		{0,0,3,2},
		{0,2,2,0},
		{2,3,0,0},
		{1,3,0,3}, // half grass/sea
		{3,0,3,1},
		{0,2,1,2},
		{2,1,2,0},
		{1,3,3,1}, // sea L corner
		{3,2,1,1},
		{2,1,1,2},
		{1,1,2,3},
		{0,0,0,0}, // rock
		{1,1,1,1}, // sea
		{4,4,4,4}, // sand
		{4,4,4,4},
		{4,4,4,4},
		{7,4,4,4}, // cactas
		{4,4,7,4},
		{6,4,4,6}, // sand L corner
		{4,4,6,5},
		{4,5,5,4},
		{5,6,4,4},
		{1,6,4,6}, // half sand/sea
		{6,4,6,1},
		{4,5,1,5},
		{5,1,5,4},
		{1,6,6,1}, // sea L corner
		{6,5,1,1},
		{5,1,1,5},
		{1,1,5,6},
		{9,4,4,9}, // sand/grass L corner
		{4,4,9,8},
		{4,8,8,4},
		{8,9,4,4},
		{0,9,4,9}, // half sand/grass
		{9,4,9,0},
		{4,8,0,8},
		{8,0,8,4},
		{0,9,9,0}, // sand/grass L corner
		{9,8,0,0},
		{8,0,0,8},
		{0,0,8,9},
		{3,9,9,3}, //  sand/grass/sea
		{8,8,3,2},
		{8,2,2,8},
		{2,3,8,9}

	};
	static constexpr short tile_weights[] = {
		200,
		6,
		6,
		6,
		6,
		6,
		6,
		6,
		6,
		6,
		6,
		6,
		6,
		10,
		400,
		140,
		10,
		10,
		10,
		10,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		8,
		15,
		15,
		15,
		15,
		15,
		15,
		15,
		15,
		15,
		15,
		15,
		15,
		2,
		2,
		2,
		2
	};

private:
	std::random_device rd;
	std::mt19937 random_gen;
};