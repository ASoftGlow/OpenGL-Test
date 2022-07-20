#pragma once

#include <vector>

#include "tile.h"


class Terrain {
public:
	Terrain(int H, int W);
	Tile* getTile(int x, int y);
	void generate();
	void collapse(int x, int y);

	unsigned height, width;

private:
	static const unsigned short total_tiles = 6;
	const char rotation_pos[4][2] = {
		 0,  1,
		 1,  0,
		 0, -1,
		-1,  0
	};
	const char opposite_rotations[4] = { 2,3,0,1 };
	const char tileRules[6][4] = {
		/*
		  0: full land
		  1: full sea
		  2,3: land/sea
		 */
		 // 0,1,2,3
		 // u,r,d,l
		   {0,0,0,0}, // land
		   {1,1,1,1}, // sea
		   {2,1,2,0}, // half land/sea
		   {0,2,2,0}, // land corner
		   {2,1,1,2}, // sea corner
		   {0,0,0,0}  // rock

	};
	std::vector<Tile> tiles;
	std::vector<std::vector<Choice>> tile_choices;

	std::vector<Choice>* getChoices(int x, int y);
	void find_easiest_choice(int &x, int &y, unsigned& size);
};