#include <limits.h>

#include "chunk_generator.h"
#include "terrain.h"
#include "terrain_renderer.h"
#include "logger.h"


ChunkGenerator::ChunkGenerator(unsigned short chunk_size, std::mt19937* random_gen)
	: chunk_size(chunk_size), random_gen(random_gen)
{
	tile_choices.resize((size_t)chunk_size * chunk_size);
};

bool ChunkGenerator::findEasiestChoice(unsigned& rX, unsigned& rY)
{
	unsigned short smallest_size = USHRT_MAX;

	for (unsigned short y = 0; y != chunk_size; y++) {
		for (unsigned short x = 0; x != chunk_size; x++)
		{
			unsigned short size = (unsigned short)getChoices(x, y)->size();
			if (size > 1 && size < smallest_size)
			{
				smallest_size = size;
				rX = x;
				rY = y;
			}
		}
	}
	// none found
	return smallest_size != USHRT_MAX;
}

std::vector<Choice>* ChunkGenerator::getChoices(unsigned x, unsigned y)
{
	return &tile_choices.at(static_cast<size_t>(y) * chunk_size + x);
}

void ChunkGenerator::collapse(Chunk* chunk, unsigned tile_x, unsigned tile_y)
{
	Tile* t = chunk->getTile(tile_x, tile_y);
	std::vector<Choice>* chs = getChoices(tile_x, tile_y);

	if (chs->size() == 1)
	{
		t->type = chs->at(0).type;
	}

	// iterator over surrounding tiles
	for (char i = 0; i != 4; i++)
	{
		int new_x = tile_x + rotation_pos[i][0];
		int new_y = tile_y + rotation_pos[i][1];

		// check out of bounds
		if (new_x < 0 || new_x >= chunk_size ||
			new_y < 0 || new_y >= chunk_size)
			continue;

		std::vector<Choice>* chs2 = getChoices(new_x, new_y);


		unsigned short old_len = (unsigned short)chs2->size();
		unsigned short len = old_len;

		// compare each Choice
		for (unsigned short h = 0; h != len; h++)
		{
			bool isMatch = false;
			const Choice chs2_t = chs2->at(h);
			// opposite side
			const char rule2 = Terrain::tile_rules[chs2_t.type][(i + 2) % 4];

			for (int j = 0; j != chs->size(); j++)
			{
				const Choice chs_t = chs->at(j);
				const char rule = Terrain::tile_rules[chs_t.type][i];

				if (rule == rule2)
				{
					isMatch = true;
					break;
				}
			}

			if (isMatch) continue;

			chs2->erase(chs2->begin() + h);

			if (chs->size() == 0)
			{
				printf("MISSING ");
				break;
			}
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

Chunk ChunkGenerator::generate(int chunk_x, int chunk_y)
{
	Logger::debug((std::string("Generating chunk (") + std::to_string(chunk_x) + ", " + std::to_string(chunk_y) + ")").c_str());

	for (size_t i = 0; i != (size_t)chunk_size * chunk_size; i++)
	{
		tile_choices[i].resize(TerrainRenderer::atlas_size);
	}

	for (unsigned short k = 0; k != chunk_size; k++) {
		for (unsigned short j = 0; j != chunk_size; j++)
		{
			for (unsigned short i = 0; i != TerrainRenderer::atlas_size; i++) {
				tile_choices[static_cast<size_t>(k) * chunk_size + j][i] = Choice{ (TileType)i };
			}
		}
	}

	// Update Choices based on existing surrounding chunks
	//for (char j = 0; j < 4; j++)
	//{
	//	const char* chunk_offset = rotation_pos[j];
	//	std::pair<int, int> chunk_pos = std::pair<int, int>(chunk_x + chunk_offset[0], chunk_y + chunk_offset[1]);
	//	if (!chunks->contains(chunk_pos))
	//		// No neighbor
	//		continue;
	//	Chunk* refChunk = &chunks->at(chunk_pos);

	//	const bool isWidth = chunk_offset[0] == 0;
	//	int x2 = (chunk_offset[0] == 1) ? chunk_size - 1 : 0;
	//	int y2 = (chunk_offset[1] == -1) ? chunk_size - 1 : 0;

	//	for (unsigned short i = 0; i < chunk_size; i++)
	//	{
	//		// Get reference tile (neighbor chunk)
	//		Tile* tile = refChunk->getTile(
	//			(isWidth ? (x2 + i) : (chunk_size - 1 - x2)),
	//			(isWidth ? (chunk_size - 1 - y2) : (y2 + i))
	//		);
	//		// Get end tile choices (current gen)
	//		std::vector<Choice>* choices = getChoices(
	//			x2 + (isWidth ? i : 0),
	//			y2 + (isWidth ? 0 : i)
	//		);

	//		char rule = Terrain::tile_rules[tile->type][isWidth ? j : (j + 2) % 4];
	//		unsigned short len = (unsigned short)choices->size();

	//		// compare each Choice
	//		for (unsigned short h = 0; h != len; h++)
	//		{
	//			Choice choice = choices->at(h);
	//			// opposite side
	//			char rule2 = Terrain::tile_rules[choice.type][isWidth ? (j + 2) % 4 : j];

	//			if (rule == rule2)
	//				continue;

	//			choices->erase(choices->begin() + h);
	//			len--;
	//			h--;
	//		}
	//	}
	//}

	Chunk chunk(chunk_x, chunk_y, chunk_size);

	while (true)
	{
		unsigned x, y;
		if (!findEasiestChoice(x, y)) break;

		// pick a choice and isolate it
		std::vector<Choice>* chs = getChoices(x, y);
		// get weights and setup random distribution
		std::vector<short> weights;
		weights.reserve(chs->size());
		for (Choice c : *chs)
		{
			weights.push_back(Terrain::tile_weights[c.type]);
		}

		std::discrete_distribution<> weighted_rand = std::discrete_distribution<>{ weights.begin(),weights.end() };

		Choice ch = chs->at(weighted_rand(*random_gen));

		chs->clear();
		chs->push_back(ch);

		collapse(&chunk, x, y);
	}

	return chunk;
}