#pragma once

#include <glm/glm.hpp>


class Game
{
public:
	void init();
	void update();
	void render();
	void end();

	float scale = 0.0f;
private:
};