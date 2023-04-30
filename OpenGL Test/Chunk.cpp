#include "Chunk.h"
#include "SaveManager.h"

Chunk::Chunk() {}

Chunk::Chunk(int x, int y, unsigned short* size) : x(x), y(y), size(size)
{
	tiles.resize((size_t)*size * *size);
}

Tile* Chunk::getTile(unsigned short x, unsigned short y) {
	return &tiles.at(static_cast<size_t>(y) * SaveManager::current.chunk_size + x);
}