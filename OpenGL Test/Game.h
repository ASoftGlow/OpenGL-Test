#pragma once

#include <glm/glm.hpp>

#include "terrain_renderer.h"
#include "sprite_renderer.h"
#include "Logger.h"


class Game
{
public:
	~Game();
	void init();
	void update();
	void render();
	void save();
	void load();

	float scale = 8.0f;
	Logger logger{};

//private:
	float pan_speed = 1.25f;
	float x = 0.f;
	float y = 0.f;
	Terrain* terrain;
};