#include <iostream>
#include <stdlib.h>

#include "terrain.h"
#include "terrain_renderer.h"
#include "utils.h"


Terrain::Terrain()
{
	random_gen = std::mt19937(rd());
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

void Terrain::generate(int chunk_x, int chunk_y) {
	unsigned x, y;
	unsigned easiest_size;
	(*chunks)[std::pair<int, int>(chunk_x, chunk_y)] = Chunk{ chunk_x, chunk_y, width };
	Chunk* chunk = &chunks->at(std::pair<int, int>(chunk_x, chunk_y));
	std::vector<Choice>* chs;
	Choice ch;
	std::discrete_distribution<> weighted_rand;

	tile_choices.resize((size_t)*height * *width);
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

	// Update Choices based on existing surrounding chunks
	for (char j = 0; j < 4; j++)
	{
		const char* chunk_offset = rotation_pos[j];
		std::pair<int, int> chunk_pos = std::pair<int, int>(chunk_x + chunk_offset[0], chunk_y + chunk_offset[1]);
		if (!chunks->contains(chunk_pos))
			// No neighbor
			continue;
		Chunk* refChunk = &chunks->at(chunk_pos);

		const bool isWidth = chunk_offset[0] == 0;
		int x2 = (chunk_offset[0] == 1) ? *width - 1 : 0;
		int y2 = (chunk_offset[1] == -1) ? *height - 1 : 0;

		for (unsigned short i = 0; i < (isWidth ? *height : *width); i++)
		{
			// Get reference tile (neighbor chunk)
			Tile* tile = refChunk->getTile(
				(isWidth ? (x2 + i) : (*width - 1 - x2)),
				(isWidth ? (*height - 1 - y2) : (y2 + i))
			);
			// Get end tile choices (current gen)
			std::vector<Choice>* choices = getChoices(
				x2 + (isWidth ? i : 0),
				y2 + (isWidth ? 0 : i)
			);

			char rule = tile_rules[tile->type][isWidth ? j : (j + 2) % 4];
			unsigned short len = choices->size();

			// compare each Choice
			for (unsigned short h = 0; h != len; h++)
			{
				Choice choice = choices->at(h);
				// opposite side
				char rule2 = tile_rules[choice.type][isWidth ? (j + 2) % 4 : j];

				if (rule == rule2)
					continue;

				choices->erase(choices->begin() + h);
				len--;
				h--;
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

		collapse(chunk, x, y);
	}

	// clean up
	this->tile_choices.clear();

	/*std::cout << std::endl;
	std::cout << "Done!" << std::endl;*/
	//for (char j = 0; j < 4; j++)
	//{
	//	const char* chunk_offset = rotation_pos[j];
	//	if (chunks->size() < 2) break;
	//	//std::cout << (int)chunk_offset[0] << " " << (int)chunk_offset[1] << std::endl;
	//	std::pair<int, int> chunk_pos = std::pair<int, int>(chunk_x + chunk_offset[0], chunk_y + chunk_offset[1]);
	//	if (!chunks->contains(chunk_pos))
	//		continue;
	//	Chunk* refChunk = &chunks->at(chunk_pos);

	//	const bool isWidth = chunk_offset[0] == 0;
	//	int x2 = (chunk_offset[0] == 1) ? *width - 1 : 0;
	//	int y2 = (chunk_offset[1] == -1) ? *height - 1 : 0;

	//	for (unsigned short i = 0; i < (isWidth ? *height : *width); i++)
	//	{
	//		// Get reference tile (neighbor chunk)
	//		refChunk->getTile(
	//			(isWidth ? (x2 + i) : (*width - 1 - x2)),
	//			(isWidth ? (*height - 1 - y2) : (y2 + i))
	//		)->type = Grass;

	//		chunk->getTile(
	//			x2 + (isWidth ? i : 0),
	//			y2 + (isWidth ? 0 : i)
	//		)->type = Sea;
	//	}
	//}
}

// TODO: account for neighboring chunks
void Terrain::collapse(Chunk* chunk, unsigned tile_x, unsigned tile_y) {
	Tile* t = chunk->getTile(tile_x, tile_y);
	std::vector<Choice>* chs = getChoices(tile_x, tile_y);
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
		new_x = tile_x + rotation_pos[i][0];
		new_y = tile_y + rotation_pos[i][1];

		// check out of bounds
		if (new_x < 0 || new_x >= *width ||
			new_y < 0 || new_y >= *height)
			continue;

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
			collapse(chunk, new_x, new_y);
		}
	}
}