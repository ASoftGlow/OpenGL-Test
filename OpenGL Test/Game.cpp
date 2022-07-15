#include "Game.h"
#include "resources.hpp"
#include "sprite_renderer.h"


SpriteRenderer* Renderer;

void Game::init()
{
    // load shaders
    Resources::loadShader("terrain.vert", "terrain.frag", "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, 800.0f,
        800.0f, 0.0f, -1.0f, 1.0f);
    Resources::getShader("sprite").use();
    Resources::getShader("sprite").setInt("image", 0);
    Resources::getShader("sprite").setMatrix4("projection", projection);
    // set render-specific controls
    Shader s = Resources::getShader("sprite");
    Renderer = new SpriteRenderer(s);
    // load textures
    Resources::loadTexture("assets/atlas.png", true, "atlas");
}


void Game::render()
{
	// tiles

    // sprites
    Texture t = Resources::getTexture("atlas");
    Renderer->DrawSprite(t, glm::vec2(200.0f, 200.0f), 200.0f, 45.0f);
}