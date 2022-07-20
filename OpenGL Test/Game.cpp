#include "Game.h"
#include "resources.hpp"
#include "sprite_renderer.h"
#include "terrain_renderer.h"


TerrainRenderer* Renderer;
std::map<unsigned, Terrain*> chunks;


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
	Resources::loadTexture("assets/atlas.png", true, "atlas");

	// gen terrain
	chunks[0] = new Terrain{ 5,5 };
	chunks[0]->generate();

	// create renderers
	Renderer = new TerrainRenderer(*Resources::getShader("sprite"), *chunks[0]);
}


void Game::render()
{	
	// terrain
	Renderer->drawTerrain(*Resources::getTexture("atlas"), 100.0f, 100.0f, this->scale, 0.0f);

	// sprites
	//Renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}