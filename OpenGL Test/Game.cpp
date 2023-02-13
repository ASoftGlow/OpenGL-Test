#include "Game.h"
#include "resources.hpp"
#include "sprite_renderer.h"


TerrainRenderer* Renderer;


Game::~Game()
{
	delete Renderer;
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
	Resources::loadTexture("assets/atlas.png", true, "terrain_atlas", false);
	Resources::loadArrayTexture("assets/tiles", "terrain");

	// gen terrain
	terrain = new Terrain{ 10,10 };
	terrain->generate();

	// create renderers
	Renderer = new TerrainRenderer(*Resources::getShader("terrain"), terrain);
}


void Game::render()
{
	// terrain
	Renderer->drawTerrain(
		*Resources::getArrayTexture("terrain"),
		400.0f,
		400.0f,
		this->scale * 8,
		0.0f);

	// sprites
	//Renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}


void Game::update()
{
	terrain->generate();
	Renderer->updateVBO();
}