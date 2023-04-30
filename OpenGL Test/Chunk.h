#pragma once

#include <vector>
#include "tile.h"

// A Chunk of tiles
class Chunk {
public:
	Chunk() : x(0), y(0) {};
	explicit Chunk(int x, int y, unsigned short* size);
	Tile* getTile(unsigned short x, unsigned short y);

	int x, y;
	unsigned short* size;
	std::vector<Tile> tiles;
};