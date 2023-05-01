#pragma once
#include <vector>
#include <random>
#include <map>

#include "chunk.h"

class ChunkGenerator
{
public:
	ChunkGenerator(unsigned short chunk_size, std::mt19937* random_gen, std::map<std::pair<int, int>, Chunk>* chunks);
	Chunk generate(int x, int y, bool blend);

private:
	std::vector<Choice>* getChoices(unsigned x, unsigned y);
	// Returns false if no choices
	bool findEasiestChoice(unsigned& x, unsigned& y);
	void collapse(Chunk* chunk, unsigned x, unsigned y);

	const unsigned short chunk_size;
	std::mt19937* random_gen;
	std::vector<std::vector<Choice>> tile_choices;
	std::map<std::pair<int, int>, Chunk>* chunks;

	static constexpr char rotation_pos[4][2] = {
		{0,  1}, // N
		{1,  0}, // E
		{0, -1}, // S
		{-1, 0}  // W
	};
};