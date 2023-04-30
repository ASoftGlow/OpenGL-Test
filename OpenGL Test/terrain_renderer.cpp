#include "terrain_renderer.h"


TerrainRenderer::TerrainRenderer(Shader shader, Terrain* terrain)
{
	this->shader = shader;
	this->terrain = terrain;

	const unsigned long len = *this->terrain->height * *this->terrain->width * 2 * 3 * 4;
	this->vertices.resize(len);

	this->initRenderData();
}


void TerrainRenderer::initRenderData()
{
	generateVertices();


	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &this->quadsVBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->quadsVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void TerrainRenderer::updateVBO(bool resize)
{
	if (resize)
	{
		const unsigned long len = *this->terrain->height * *this->terrain->width * 2 * 3 * 4;
		this->vertices.resize(len);
	}

	generateVertices();

	glBindBuffer(GL_ARRAY_BUFFER, this->quadsVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void TerrainRenderer::generateVertices()
{
	Tile* tile;
	unsigned long long ind;

	// gen vertices
	for (unsigned y = 0; y != *this->terrain->height; y++) {
		for (unsigned x = 0; x != *this->terrain->width; x++)
		{
			tile = this->terrain->getTile(x, y);

			// 2 tri, 3 vert per
			ind = (x + y * *this->terrain->width) * 24;

			for (char j = 0; j < 12; j += 2)
			{
				// pos
				this->vertices[ind + j * 2 + 0] = quad_pos[j + 0] + x;
				this->vertices[ind + j * 2 + 1] = quad_pos[j + 1] + *this->terrain->height - y;

				// tex
				const char* tile_tex_pos = tiles_tex_pos[tile->type];
				// Account for gap
				this->vertices[ind + j * 2 + 2] =
					((short)tile_tex_pos[0] * (tile_size + 2) + quad_tex[j + 0] * tile_size + 1) / (atlas_width * (tile_size + 2));

				this->vertices[ind + j * 2 + 3] =
					((short)tile_tex_pos[1] * (tile_size + 2) + quad_tex[j + 1] * tile_size + 1) / (atlas_height * (tile_size + 2));
			}
		}
	}
}


void TerrainRenderer::drawTerrain(Texture& atlas, float x, float y, float scale, float rotation)
{
	x -= scale * *this->terrain->width / 2;
	y -= scale * *this->terrain->height / 2;
	y -= scale;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x, y, 0.0f));

	model = glm::translate(model, glm::vec3(0.5f * scale, 0.5f * scale, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * scale, -0.5f * scale, 0.0f));

	model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

	this->shader.setMatrix4("model", model);

	glActiveTexture(GL_TEXTURE0);
	atlas.bind();

	unsigned long len = *this->terrain->height * *this->terrain->width * 3 * 2;

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, len);

	glBindVertexArray(0);
}
