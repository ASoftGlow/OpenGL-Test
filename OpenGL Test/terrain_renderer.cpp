#include "terrain_renderer.h"

TerrainRenderer::TerrainRenderer(Shader shader, Terrain* terrain)
{
	this->shader = shader;
	this->terrain = terrain;

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
		const size_t len = this->terrain->chunks->size() * *this->terrain->height * *this->terrain->width * 2 * 3 * 4;
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
	size_t ind;
	size_t count = 0;

	for (auto& [pos, chunk] : *this->terrain->chunks)
	{
		// gen vertices
		for (unsigned y = 0; y != *this->terrain->height; y++) {
			for (unsigned x = 0; x != *this->terrain->width; x++)
			{
				tile = chunk.getTile(x, y);

				// 2 tri, 3 vert per
				ind = (x + y * *this->terrain->width) * 2 * 3 * 4 + (count * *this->terrain->height * *this->terrain->width * 2 * 3 * 4);

				for (char j = 0; j < 12; j += 2)
				{
					// pos
					this->vertices[ind + j * 2 + 0] = quad_pos[j + 0] + x;// + pos.first * *chunk.size * *chunk.size;
					this->vertices[ind + j * 2 + 1] = quad_pos[j + 1] + y;// + pos.second * *chunk.size * *chunk.size;

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
		count++;
	}
}


void TerrainRenderer::drawTerrain(Texture& atlas, float x, float y, float scale, float rotation)
{
	float newX, newY;
	glm::mat4 model;
	const size_t len = *this->terrain->height * *this->terrain->width * 3 * 2;
	size_t count = 0;

	glActiveTexture(GL_TEXTURE0);
	atlas.bind();
	glBindVertexArray(this->quadVAO);

	for (auto& [pos, chunk] : *this->terrain->chunks)
	{
		newX = x - scale * (*this->terrain->width / 2 - pos.first * *this->terrain->width);
		newY = y - scale * (*this->terrain->height / 2 + pos.second * *this->terrain->height - 1);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(newX, newY, 0.0f));

		model = glm::translate(model, glm::vec3(0.5f * scale, 0.5f * scale, 0.0f));
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::translate(model, glm::vec3(-0.5f * scale, -0.5f * scale, 0.0f));

		model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

		this->shader.setMatrix4("model", model);

		glDrawArrays(GL_TRIANGLES, count * len, len);
		count++;
	}

	glBindVertexArray(0);
}
