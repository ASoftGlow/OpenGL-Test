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
	Resources::loadShader("terrain.vert", "terrain.frag", "sprite");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, 800.0f,
		800.0f, 0.0f, -1.0f, 1.0f);
	Resources::getShader("sprite")->use();
	Resources::getShader("sprite")->setInt("image", 0);
	Resources::getShader("sprite")->setMatrix4("projection", projection);
	
	// load textures
	Resources::loadTexture("assets/atlas.png", true, "atlas", true);

	// gen terrain
	chunk = new Terrain{ 5,5 };
	chunk->generate();

	// create renderers
	Renderer = new TerrainRenderer(*Resources::getShader("sprite"), chunk);
}


void Game::render()
{	
	// terrain
	Renderer->drawTerrain(
		*Resources::getTexture("atlas"),
		400.0f,
		400.0f,
		this->scale * 8,
		0.0f);

	// sprites
	//Renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}


void Game::update()
{
	chunk->generate();
	Renderer->updateVBO();
}