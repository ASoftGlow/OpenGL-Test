#pragma once

#include <glm/glm.hpp>


class Game
{
public:
	~Game();
	void init();
	void update();
	void render();
	void end();

	float scale = 128.0f;

private:
};