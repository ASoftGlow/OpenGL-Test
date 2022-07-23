#include <iostream>
#include <stdlib.h>

#include "terrain.h"
#include "tile.h"
#include "utils.h"


Terrain::Terrain(int H, int W)
{
	height = H;
	width = W;

	tiles.resize(static_cast<size_t>(height) * width);
}


Tile* Terrain::getTile(int x, int y) {
	return &tiles.at(static_cast<size_t>(y) * width + x);
}


std::vector<Choice>* Terrain::getChoices(int x, int y) {
	return &tile_choices.at(static_cast<size_t>(y) * width + x);
}


void Terrain::find_easiest_choice(int& x, int& y, unsigned& size)
{
	size = total_tiles * 4 + 1;

	for (int k = 0; k != height; k++) {
		for (int j = 0; j != width; j++)
		{
			unsigned short size2 = getChoices(j, k)->size();
			if (size2 < size && size2 > 1)
			{
				size = size2;
				x = j;
				y = k;
			}
		}
	}

	if (size == total_tiles * 4 + 1) size = 1;
}


void Terrain::generate() {
	/*tiles = {
		{2,2},{3,0},{4,0},{1,0},
		{3,2},{2,0},{1,0},{1,0},
		{0,0},{3,3},{4,3},{1,0},
		{0,0},{0,0},{2,0},{1,0}
	};
	tiles = {
		{2,2},{3,3},{4,3},{1,0},
		{3,1},{2,0},{1,0},{1,0},
		{0,0},{3,0},{4,0},{1,0},
		{0,0},{0,0},{2,0},{1,0}
	};
	return;*/
	int x, y;
	unsigned easiest_size;
	std::vector<Choice>* chs;
	Choice ch;

	tile_choices.resize(static_cast<size_t>(height) * width);

	// account for rotation
	for (size_t i = 0; i != static_cast<size_t>(height) * width; i++)
	{
		tile_choices[i].resize(static_cast<int>(total_tiles) * 4);
	}


	for (int k = 0; k != height; k++) {
		for (int j = 0; j != width; j++)
		{
			for (unsigned short i = 0; i != total_tiles; i++) {
				for (char h = 0; h != 4; h++)
				{
					tile_choices[static_cast<size_t>(k) * width + j][static_cast<int>(i) * 4 + h] = Choice{ i, h };
				}
			}
		}
	}


	while (true)
	{
		find_easiest_choice(x, y, easiest_size);

		if (easiest_size == 1) break;

		// pick a choice and isolate it
		chs = getChoices(x, y);
		ch = chs->at(rand() % chs->size()); // TODO: weight

		chs->clear();
		chs->push_back(ch);

		collapse(x, y);
	}


	// clean up
	this->tile_choices.clear();

	//std::cout << "Done!" << std::endl;
}


void Terrain::collapse(int x, int y) {
	Tile* t = getTile(x, y);
	std::vector<Choice>* chs = getChoices(x, y);
	std::vector<Choice>* chs2;
	Choice chs_t, chs2_t;
	int x_n, y_n;
	unsigned short old_len, len;
	bool q;
	char rule, rule2;


	if (chs->size() == 1)
	{
		t->type = chs->at(0).type;
		t->rotation = chs->at(0).rotation;
	}

	// iterator over surrounding tiles
	for (char i = 0; i != 4; i++)
	{
		x_n = x + rotation_pos[i][0];
		y_n = y + rotation_pos[i][1];

		// out of bounds
		if (x_n < 0 || x_n >= width ||
			y_n < 0 || y_n >= height) continue;

		chs2 = getChoices(x_n, y_n);

		old_len = len = chs2->size();

		// compare each Choice
		for (unsigned short h = 0; h != len; h++)
		{
			q = false;
			chs2_t = chs2->at(h);
			rule2 = tileRules[chs2_t.type][mod(opposite_rotations[i] - chs2_t.rotation, 4)];
			if (rule2 == 2) rule2 += (chs2_t.rotation < 2);


			for (int j = 0; j != chs->size(); j++)
			{
				chs_t = chs->at(j);
				rule = tileRules[chs_t.type][mod(i - chs_t.rotation, 4)];
				if (rule == 2) rule += (chs_t.rotation < 2);

				if (rule == rule2)
				{
					q = true;
					break;
				}
			}

			if (q) continue;


			chs2->erase(chs2->begin() + h);

			if (chs->size() == 1)
			{
				t->type = chs->at(0).type;
				t->rotation = chs->at(0).rotation;
			}

			h--;
			len--;
		}


		if (len != old_len) {
			collapse(x_n, y_n);
		}
	}
}