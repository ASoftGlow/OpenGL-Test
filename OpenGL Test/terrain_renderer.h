#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_c.h"
#include "texture_c.h"
#include "terrain.h"


class TerrainRenderer
{
public:
	TerrainRenderer(Shader &shader, Terrain &terrain);
	//~TerrainRenderer();

	void drawTerrain(Texture& atlas, float x, float y, float scale = 1.0f, float rotation = 0.0f);	

	Terrain *terrain;
private:
	Shader shader;
	std::vector<float> vertices;
	unsigned int quadVAO;

	void initRenderData();
};