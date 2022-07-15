#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
//layout (location = 1) in float tile;

out vec2 TexCoord;

//uniform vec2 scale;
uniform mat4 model;
uniform mat4 projection;

void main()
{
    TexCoord = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}