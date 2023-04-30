#include <iostream>
#include <stdlib.h>

#include "terrain.h"
#include "terrain_renderer.h"


Terrain::Terrain()
{
	random_gen = std::mt19937(rd());
}


Tile* Terrain::getTile(unsigned x, unsigned y) {
	return &tiles->at(static_cast<size_t>(y) * *width + x);
}


std::vector<Choice>* Terrain::getChoices(unsigned x, unsigned y) {
	return &tile_choices.at(static_cast<size_t>(y) * *width + x);
}


void Terrain::findEasiestChoice(unsigned& x, unsigned& y, unsigned& size)
{
	size = TerrainRenderer::atlas_size * 4 + 1;
	unsigned short size2;

	for (int k = 0; k != *height; k++) {
		for (int j = 0; j != *width; j++)
		{
			size2 = (unsigned short)getChoices(j, k)->size();
			if (size2 < size && size2 > 1)
			{
				size = size2;
				x = j;
				y = k;
			}
		}
	}

	if (size == TerrainRenderer::atlas_size * 4 + 1) size = 1;
}

void Terrain::generate(unsigned width, unsigned height)
{
	*this->width = width;
	*this->height = height;
	tiles->resize(static_cast<size_t>(height) * width);

	generate();
}

void Terrain::generate() {
	unsigned x, y;
	unsigned easiest_size;
	std::vector<Choice>* chs;
	Choice ch;
	;
	std::discrete_distribution<> weighted_rand;

	tile_choices.resize((size_t)*height * *width);

	// account for rotation
	for (size_t i = 0; i != (size_t)*height * *width; i++)
	{
		tile_choices[i].resize(TerrainRenderer::atlas_size);
	}

	for (int k = 0; k != *height; k++) {
		for (int j = 0; j != *width; j++)
		{
			for (unsigned short i = 0; i != TerrainRenderer::atlas_size; i++) {
				tile_choices[static_cast<size_t>(k) * *width + j][i] = Choice{ (TileType)i };
			}
		}
	}


	while (true)
	{
		findEasiestChoice(x, y, easiest_size);
		if (easiest_size == 1) break;

		// pick a choice and isolate it
		chs = getChoices(x, y);
		// get weights and setup random distribution
		std::vector<char> weights;
		weights.reserve(chs->size());
		for (Choice c : *chs)
		{
			weights.push_back(tile_weights[c.type]);
		}

		weighted_rand = std::discrete_distribution<>{ weights.begin(),weights.end() };

		ch = chs->at(weighted_rand(random_gen));

		chs->clear();
		chs->push_back(ch);

		collapse(x, y);
	}

	// clean up
	this->tile_choices.clear();

	//for (int i = 0; i != this->tiles.size(); i++) {
	//	std::cout << this->tiles[i].type << " ";
	//}
	//std::cout << std::endl;
	//std::cout << "Done!" << std::endl;
}


void Terrain::collapse(unsigned x, unsigned y) {
	Tile* t = getTile(x, y);
	std::vector<Choice>* chs = getChoices(x, y);
	std::vector<Choice>* chs2;
	Choice chs_t, chs2_t;
	int new_x, new_y;
	unsigned short old_len, len;
	bool isMatch;
	char rule, rule2;


	if (chs->size() == 1)
	{
		t->type = chs->at(0).type;
	}

	// iterator over surrounding tiles
	for (char i = 0; i != 4; i++)
	{
		new_x = x + rotation_pos[i][0];
		new_y = y + rotation_pos[i][1];

		// check out of bounds
		if (new_x < 0 || new_x >= *width ||
			new_y < 0 || new_y >= *height) continue;

		chs2 = getChoices(new_x, new_y);

		old_len = len = (unsigned short)chs2->size();

		// compare each Choice
		for (unsigned short h = 0; h != len; h++)
		{
			isMatch = false;
			chs2_t = chs2->at(h);
			// opposite side
			rule2 = tile_rules[chs2_t.type][(i + 2) % 4];

			for (int j = 0; j != chs->size(); j++)
			{
				chs_t = chs->at(j);
				rule = tile_rules[chs_t.type][i];

				if (rule == rule2)
				{
					isMatch = true;
					break;
				}
			}

			if (isMatch) continue;


			chs2->erase(chs2->begin() + h);

			if (chs->size() == 1)
			{
				t->type = chs->at(0).type;
			}

			h--;
			len--;
		}

		if (len != old_len) {
			collapse(new_x, new_y);
		}
	}
}