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
	Resources::loadShader("terrain.vert", "terrain.frag", "terrain");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, 800.0f,
		800.0f, 0.0f, -1.0f, 1.0f);
	Resources::getShader("terrain")->use();
	Resources::getShader("terrain")->setInt("image", 0);
	Resources::getShader("terrain")->setMatrix4("projection", projection);

	// load textures
	Resources::loadTexture("assets/atlas2.png", true, "terrain_atlas", false);

	// random_gen terrain
	terrain = new Terrain{
		&SaveManager::current.chunk_size,
		&SaveManager::current.chunks
	};

	// create renderers
	renderer = new TerrainRenderer(*Resources::getShader("terrain"), terrain);

	/*SaveManager::current.chunk_size = 16;

	create();
	terrain->generate(1, 0);
	terrain->generate(0, 1);
	terrain->generate(1, 1);*/
	SaveManager::load(1);
	SaveManager::loadChunk(0, 0);
	SaveManager::loadChunk(1, 0);
	SaveManager::loadChunk(0, 1);
	SaveManager::loadChunk(1, 1);
	renderer->updateVBO();
}


void Game::render()
{
	// terrain
	renderer->drawTerrain(
		*Resources::getTexture("terrain_atlas"),
		400.0f + this->x,
		400.0f + this->y,
		this->scale,
		0.0f);

	// sprites
	//renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}


void Game::update()
{
	/*const int x = (int)floor(this->x / *terrain->width);
	const int y = (int)floor(this->y / *terrain->height);
	//std::cout << "x: " << this->x << "\t(" << x << ")\ty: " << this->y << "\t(" << y << ")" << std::endl;
	Chunk* chunk = terrain->getChunk(x, y);
	if (chunk == nullptr)
	{
		terrain->generate(x, y);
		renderer->updateVBO();
	}*/
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
	count++;
	SaveManager::current.id = count;
	terrain->chunks->clear();

	// First chunk
	terrain->generate(0, 0);
}