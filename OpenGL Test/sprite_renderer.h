#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_c.h"
#include "texture_c.h"


class SpriteRenderer
{
public:
    SpriteRenderer(Shader& shader);
    //~SpriteRenderer();

    void DrawSprite(Texture &texture, glm::vec2 position, float size = 10.0f, float rotation = 0.0f);

private:
    Shader       shader;
    unsigned int quadVAO;

    void initRenderData();
};