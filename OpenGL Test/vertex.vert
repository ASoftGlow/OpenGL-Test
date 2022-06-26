#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float tile;

out vec2 TexCoord;

uniform float scale_x;
uniform float scale_y;


void main()
{
   float x = aPos.x * scale_x;
   float y = aPos.y * scale_y;

   gl_Position = vec4(x, y, 0, 1.0);
   TexCoord = aTexCoord;
}