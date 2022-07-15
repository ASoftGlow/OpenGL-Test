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

	void drawTerrain(Texture& atlas, glm::vec2 pos, float scale = 1.0f, float rotation = 0.0f);	

private:
	Shader shader;
	Terrain *terrain;
	float *vertices;
	unsigned int quadVAO;

	void initRenderData();
};