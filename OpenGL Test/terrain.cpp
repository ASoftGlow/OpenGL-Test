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
					tile_choices[static_cast<size_t>(k) * width + j][static_cast<int>(i) * 4 + h] = Choice{ i, h, 0 };
				}
			}
		}
	}
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
	int x, y;
	unsigned easiest_size;
	std::vector<Choice>* chs;
	Choice ch;


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
	/*for (int k = 0; k != height; k++) {
		for (int j = 0; j != width; j++)
		{
			tile_choices.
		}
	}*/

	std::cout << "Done!" << std::endl;
}


void Terrain::collapse(int x, int y) {
	std::vector<Choice>* chs = getChoices(x, y);
	Tile* t = getTile(x, y);

	if (chs->size() == 1)
	{
		t->type = chs->at(0).type;
		t->rotation = chs->at(0).rotation;
	}


	int x_n, y_n;
	std::vector<Choice>* chs2;
	unsigned short old_len, len;
	bool q;
	char rule, rule2;

	for (char i = 0; i != 4; i++)
	{
		x_n = x + rotation_pos[i][0];
		y_n = y + rotation_pos[i][1];

		// out of bounds
		if (x_n < 0 || x_n >= width ||
			y_n < 0 || y_n >= height) continue;

		chs2 = getChoices(x_n, y_n);

		/*for (int k = 0; k != height; k++) {
			for (int j = 0; j != width; j++)
			{
				chs2
			}
		}*/

		old_len = len = chs2->size();

		for (unsigned short h = 0; h != len; h++)
		{
			q = 0;
			Choice chs2_t = chs2->at(h);
			rule2 = tileRules[chs2_t.type][mod(opposite_rotations[i] - chs2_t.rotation, 4)];
			if (rule2 == 2) rule2 += (chs2_t.rotation < 2);

			for (int j = 0; j != chs->size(); j++)
			{
				Choice chs_t = chs->at(j);
				rule = tileRules[chs_t.type][mod(i - chs_t.rotation, 4)];
				if (rule == 2) rule += (chs_t.rotation < 2);

				if (rule == rule2)
				{
					q = 1;
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