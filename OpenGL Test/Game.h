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
	void save(int id);
	void load(int id);
	void loadVisableChunks();
	Chunk* getCurrentChunk();
	Tile* getCurrentTile();

	int window_width, window_height;
	float zoom = 4.f;

//private:
	float pan_speed = 2.0f;
	double pan_x = 0.0;
	double pan_y = 10.0;
	double x = 0.0;
	double y = 0.0;
	Terrain* terrain;
	Logger logger{};
	int count = 0;
};