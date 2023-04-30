#pragma once

#include <vector>
#include <map>
#include <iomanip>
#include <random>

#include "Chunk.h"

class Terrain {
public:
	Terrain();
	//Tile* getTile(unsigned x, unsigned y);
	/*
	*/
	void generate(int chunk_x, int chunk_y);
	void collapse(Chunk* chunk, unsigned x, unsigned y);

	unsigned short* height, * width;
	std::map<std::pair<int, int>, Chunk>* chunks;

private:
	std::vector<Choice>* getChoices(unsigned x, unsigned y);
	void findEasiestChoice(unsigned& x, unsigned& y, unsigned& size);

	std::vector<std::vector<Choice>> tile_choices;

	std::random_device rd;
	std::mt19937 random_gen;


	static constexpr char rotation_pos[4][2] = {
		{0,  1}, // N
		{1,  0}, // E
		{0, -1}, // S
		{-1, 0}  // W
	};
	static constexpr char tile_rules[][4] = {
		/*
		0: full land
		1: full sea
		2,3: land/sea
		*/
		// 0,1,2,3
	  // S,E,N,W
		{0,0,0,0}, // land
		{3,0,0,3}, // land L corner
		{0,0,3,2},
		{0,2,2,0},
		{2,3,0,0},
		{1,3,0,3}, // half land/sea
		{3,0,3,1},
		{0,2,1,2},
		{2,1,2,0},
		{1,3,3,1}, // sea L corner
		{3,2,1,1},
		{2,1,1,2},
		{1,1,2,3},
		{0,0,0,0},  // rock
		{1,1,1,1} // sea

	};
	static constexpr char tile_weights[] = {
		10,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		15,
	};
};