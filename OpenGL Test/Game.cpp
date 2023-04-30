#include "Game.h"
#include "resources.hpp"


TerrainRenderer* renderer;


Game::~Game()
{
	delete renderer;
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
	terrain = new Terrain{};
	terrain->width = &SaveManager::current.chunk_size;
	terrain->height = &SaveManager::current.chunk_size;
	terrain->chunks = &SaveManager::current.chunks;

	// create renderers
	renderer = new TerrainRenderer(*Resources::getShader("terrain"), terrain);

	create(16);
	terrain->generate(1, 0);
	terrain->generate(0, 1);
	terrain->generate(1, 1);
	renderer->updateVBO(true);
}


void Game::render()
{
	// terrain
	renderer->drawTerrain(
		*Resources::getTexture("terrain_atlas"),
		400.0f + this->x * this->scale,
		400.0f + this->y * this->scale,
		this->scale * 8,
		0.0f);

	// sprites
	//renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}


void Game::update()
{
	create(SaveManager::current.chunk_size);
	renderer->updateVBO(false);
}

void Game::save()
{
	SaveManager::save();
	logger.info("Game saved");
}

void Game::load()
{
	// Skip is already loaded
	//if (SaveManager::current.id == count) return;
	SaveManager::load(count);
	renderer->updateVBO(true);
	logger.info("Game loaded");
}

void Game::create(unsigned short chunk_size)
{
	count++;
	SaveManager::current.id = count;
	*terrain->width = chunk_size;
	*terrain->height = chunk_size;
	terrain->chunks->clear();

	// First chunk
	terrain->generate(0, 0);
}