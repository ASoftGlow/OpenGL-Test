#pragma once

#include <glm/glm.hpp>

#include "terrain_renderer.h"


class Game
{
public:
	~Game();
	void init();
	void update();
	void render();
	void end();

	float scale = 16.0f;
	Terrain* terrain;
};