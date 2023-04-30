#include <iostream>
#include <stdlib.h>
#include <ctpl/ctpl_stl.h>

#include "terrain.h"
#include "terrain_renderer.h"
#include "save_manager.h"
#include "chunk_generator.h"

Terrain::Terrain(unsigned short* pSize, std::map<std::pair<int, int>, Chunk>* chunks_ptr)
	: chunk_size(pSize), chunks(chunks_ptr)
{
	random_gen = std::mt19937(rd());
}

void Terrain::init() {}

Chunk* Terrain::getChunk(int x, int y)
{
	const std::pair<int, int> chunk_pos = std::pair<int, int>(x, y);
	if (!chunks->contains(chunk_pos)) {
		bool success = SaveManager::loadChunk(x, y);
		if (!success)
			return nullptr;
	}
	return &chunks->at(chunk_pos);
}

void Terrain::generateChunk(int x, int y)
{
	ChunkGenerator cg(*chunk_size, &random_gen);
	SaveManager::current.chunks[std::pair<int, int>(x, y)] = cg.generate(x, y);
}

void Terrain::generateChunks(std::vector<std::pair<int, int>> chunks)
{
	std::vector<std::pair<int, int>> pass1, pass2;
	pass1.reserve(chunks.size() / 2);
	pass2.reserve(chunks.size() / 2);

	for (std::pair<int, int> c : chunks)
	{
		if (((long)c.first + (c.second % 2 == 0)) % 2 == 0)
			pass1.push_back(c);
		else
			pass2.push_back(c);
	}

	ctpl::thread_pool p(std::thread::hardware_concurrency());

	// first pass
	for (auto& [x, y] : pass1)
		p.push([&, this](int id) { generateChunk(x, y); });

	// second pass
	for (auto& [x, y] : pass2)
		p.push([&, this](int id) { generateChunk(x, y); });

	p.stop(true);
}