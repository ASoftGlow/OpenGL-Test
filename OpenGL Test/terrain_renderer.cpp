#include "terrain_renderer.h"

const unsigned char TOTAL_TILES = 6;


TerrainRenderer::TerrainRenderer(Shader& shader, Terrain& terrain)
{
	this->shader = shader;
	this->terrain = &terrain;

	unsigned long len = this->terrain->height * this->terrain->width * 3 * 2 * 4;
	this->vertices.resize(len);

	this->initRenderData();
}


void TerrainRenderer::initRenderData()
{
	unsigned VBO;
	unsigned long ind;
	Tile* tile;
	float scale_x = 1.0f / (this->terrain->width * 8);
	float scale_y = 1.0f / (this->terrain->height * 8);
	float offset_x = this->terrain->width / -2.0f;
	float offset_y = this->terrain->height / -2.0f;
	float scale = 1.0f;
	float pos_x, pos_y;
	glm::vec4 pos;
	glm::mat4 model;

	const float quad_pos[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	const float quad_tex[] = {
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	//unsigned long len = this->terrain->height * this->terrain->width * 3 * 2 * 4;

	const glm::mat4 projection = glm::ortho(0.0f, 800.0f,
		800.0f, 0.0f, -1.0f, 1.0f);

	// gen vertices
	for (unsigned int y = 0; y != this->terrain->height; y++) {
		for (unsigned int x = 0; x != this->terrain->width; x++)
		{
			tile = this->terrain->getTile(x, y);

			// transformations
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x, y, 0.0f));

			model = glm::translate(model, glm::vec3(0.5f * scale, 0.5f * scale, 0.0f));
			model = glm::rotate(model, glm::radians(90.0f * tile->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::translate(model, glm::vec3(-0.5f * scale, -0.5f * scale, 0.0f));

			model = glm::scale(model, glm::vec3(scale, scale, 1.0f));


				// 2 tri, 3 vert per
			ind = (x + y * this->terrain->width) * 24;

			for (char j = 0; j < 12; j+=2)
			{
				pos_x = quad_pos[j + 0];
				pos_y = quad_pos[j + 1];

				pos = model * glm::vec4(pos_x, pos_y, 0.0f, 1.0f);
				
				// pos
				this->vertices[ind + j*2 + 0] = pos.x;
				this->vertices[ind + j*2 + 1] = pos.y;

				// tex
				this->vertices[ind + j*2 + 2] = (quad_tex[j + 0] + tile->type) / TOTAL_TILES;
				this->vertices[ind + j*2 + 3] = quad_tex[j + 1];
			}   
			
		}
	}


	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void TerrainRenderer::drawTerrain(Texture& atlas, float x, float y, float scale, float rotation)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x, y, 0.0f));

	model = glm::translate(model, glm::vec3(0.5f * scale, 0.5f * scale, 0.0f));
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * scale, -0.5f * scale, 0.0f));

	model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

	this->shader.setMatrix4("model", model);

	glActiveTexture(GL_TEXTURE0);
	atlas.bind();

	unsigned long len = this->terrain->height * this->terrain->width * 3 * 2;

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, len);

	glBindVertexArray(0);
}
