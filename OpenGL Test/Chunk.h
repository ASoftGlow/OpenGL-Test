#pragma once
#include <vector>

#include "tile.h"

// A Chunk of tiles
struct Chunk {
public:
	Chunk() {};
	Chunk(int x, int y, unsigned short size);
	Tile* getTile(unsigned short x, unsigned short y);

	int x, y;
	std::vector<Tile> tiles;
};