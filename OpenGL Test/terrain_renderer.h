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
	TerrainRenderer(Shader &shader, Terrain* terrain);
	//~TerrainRenderer();

	void drawTerrain(Texture& atlas, float x, float y, float scale = 1.0f, float rotation = 0.0f);	
	void updateVBO();

	Terrain *terrain;
private:
	void initRenderData();
	void generateVertices();

	Shader shader;
	std::vector<float> vertices;
	unsigned int quadVAO;
	unsigned int quadsVBO;
	static constexpr float quad_pos[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};
	static constexpr float quad_tex[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};
};