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
	TerrainRenderer(Shader shader, Terrain* terrain);

	void drawTerrain(Texture& atlas, float x, float y, float scale = 1.0f, float rotation = 0.0f);
	void updateVBO();

	Terrain* terrain;

	static const char atlas_width = 5;
	static const char atlas_height = 3;
	static const unsigned atlas_size = atlas_width * atlas_height;
	static const char atlas_gap = 1;
	static const char tile_size = 8;

private:
	void initRenderData();
	void generateVertices();

	Shader shader;
	std::vector<float> vertices;
	unsigned int quadVAO;
	unsigned int quadsVBO;

	// Whole coordinates of each tile type in the atlas
	static constexpr char tiles_tex_pos[][2] = {
		{1,0},
		{4,0},
		{4,2},
		{2,2},
		{2,0},
		{3,0},
		{4,1},
		{3,2},
		{2,1},
		{0,2},
		{0,1},
		{1,1},
		{1,2},
		{0,0},
		{3,1}
	};
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