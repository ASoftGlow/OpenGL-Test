#include "terrain_renderer.h"


TerrainRenderer::TerrainRenderer(Shader& shader, Terrain &terrain)
{
	this->shader = shader;
    this->terrain = &terrain;

    const int len = this->terrain->height * this->terrain->width * 3 * 2 * 4;
    this->vertices = new float[len];

	this->initRenderData();
}


void TerrainRenderer::initRenderData()
{
    unsigned VBO, ind;
    Tile* tile;
    float scale_x = 1.0f / (this->terrain->width * 8);
    float scale_y = 1.0f / (this->terrain->height * 8);
    float offset_x = this->terrain->width / -2.0f;
    float offset_y = this->terrain->height / -2.0f;
    float scale = 0.5f;
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

            &this->vertices;
            *this->vertices;

            for (unsigned long i = 0; i != sizeof(*this->vertices) / sizeof(float); i+=24)
            {
                // 2 tri, 3 vert per
                for (char j = 0; j != 12; j+=2)
                {
                    ind = x + y * this->terrain->width;

                    pos_x = quad_pos[i + j + 0];
                    pos_y = quad_pos[i + j + 1];

                    pos = model * glm::vec4(pos_x, pos_y, 0.0f, 1.0f);

                    // pos
                    this->vertices[ind + i + j + 0] = pos.x;
                    this->vertices[ind + i + j + 1] = pos.y;

                    // pos
                    this->vertices[ind + i + j + 0] = quad_tex[i + j + 0];
                    this->vertices[ind + i + j + 1] = quad_tex[i + j + 1];   
                }   
            }
        }
    }

    

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void TerrainRenderer::drawTerrain(Texture& atlas, glm::vec2 pos, float scale, float rotation)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * scale, 0.5f * scale, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * scale, -0.5f * scale, 0.0f));

    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

    this->shader.setMatrix4("model", model);

    glActiveTexture(GL_TEXTURE0);
    atlas.bind();

    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}
