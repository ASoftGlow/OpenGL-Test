#pragma once

#include <glm/glm.hpp>

#include "terrain_renderer.h"
#include "sprite_renderer.h"
#include "Logger.h"
#include "SaveManager.h"


class Game
{
public:
	~Game();
	void init();
	void update();
	void render();
	void create();
	void save();
	void load();

	float scale = 8.0f;

//private:
	float pan_speed = 1.25f;
	float x = 0.f;
	float y = 0.f;
	Terrain* terrain;
	Logger logger{};
	int count = 0;
};