#include "Game.h"
#include "resources.hpp"


TerrainRenderer* renderer;


Game::~Game()
{
	delete renderer;
	delete terrain;
}


void Game::init()
{
	// load shaders
	Shader* terrain_shader = Resources::loadShader("terrain.vert", "terrain.frag", "terrain");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, (float)window_width,
		(float)window_height, 0.0f, -1.0f, 1.0f);
	terrain_shader->use();
	terrain_shader->setInt("image", 0);
	terrain_shader->setMatrix4("projection", projection);

	// load textures
	Resources::loadTexture("assets/atlas2.png", true, "terrain_atlas", false);
	terrain_shader->setInt("terrain_atlas", 0);
	Resources::loadTexture("assets/foliage_mask.png", true, "foliage_mask", false);
	terrain_shader->setInt("foliage_mask", 1);

	// random_gen terrain
	terrain = new Terrain{
		&SaveManager::current.chunk_size,
		&SaveManager::current.chunks
	};

	// create renderers
	renderer = new TerrainRenderer(*terrain_shader, terrain);

	//SaveManager::current.chunk_size = 16;

	//create();
	SaveManager::load(2);
	SaveManager::loadChunk(0, 0);
	SaveManager::loadChunk(1, 0);
	SaveManager::loadChunk(0, 1);
	SaveManager::loadChunk(1, 1);
	//loadVisableChunks();
	renderer->updateVBO();
}


void Game::loadVisableChunks()
{
	const int current_chunk_x = -(int)floor(zoom * this->x / *terrain->width / 8 + 0.5);
	const int current_chunk_y = (int)floor(zoom * this->y / *terrain->height / 8 + 0.5);

	const float offset_from_chunk_x = this->x - ((current_chunk_x - 0.5) * *terrain->width * 8);
	const float offset_from_chunk_y = this->y - ((current_chunk_y - 0.5) * *terrain->height * 8);

	const int view_chunk_length = window_width / 8 / *terrain->width / zoom + 2;// + (offset_from_chunk_x >= (*terrain->width * 8));
	const int view_chunk_height = window_height / 8 / *terrain->height / zoom + 2;// +();
	//std::cout << view_chunk_length << std::endl;
	for (int j = 0; j < view_chunk_height + (view_chunk_height % 2 == 0); j++)
	{
		if (j == -1) std::cout << j;
		for (int k = 0; k < view_chunk_length + (view_chunk_length % 2 == 0); k++)
		{
			int y = current_chunk_y - floor(view_chunk_height / 2) + j;
			int x = current_chunk_x - floor(view_chunk_length / 2) + k;

			//std::cout << "x: " << x << " y: " << y << std::endl;
			Chunk* chunk = terrain->getChunk(x, y);
			if (chunk == nullptr)
			{
				std::cout << "generating chunk at " << x << " " << y << std::endl;
				terrain->generate(x, y);
			}
		}
	}
}


void Game::render()
{
	// terrain
	renderer->drawTerrain(
		*Resources::getTexture("terrain_atlas"),
		*Resources::getTexture("foliage_mask"),
		this->x * 1 * zoom + (float)window_width / 2,
		this->y * 1 * zoom + (float)window_height / 2,
		this->zoom * 8,
		0.0f);

	// sprites
	//renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}


void Game::update()
{

	//std::cout << "x: " << this->x << "\t(" << x << ")\ty: " << this->y << "\t(" << y << ")" << std::endl;
	//terrain->chunks->clear();
	//loadVisableChunks();
	//renderer->updateVBO();

}

void Game::save()
{
	SaveManager::save();
	logger.info("Game saved");
}

void Game::load()
{
	SaveManager::load(count);
	renderer->updateVBO();
	logger.info("Game loaded");
}

void Game::create()
{
	SaveManager::newSave(2);
	terrain->init();
	// First chunks
	//while (1)
	terrain->generate(0, 0);
	terrain->generate(1, 0);
	terrain->generate(0, 1);
	terrain->generate(1, 1);

	renderer->updateVBO();
}