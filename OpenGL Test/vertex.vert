#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float tile;

out vec2 TexCoord;

uniform float scale;
uniform int window_w;
uniform int window_h;


void main()
{
   float x = aPos.x / (window_w/100) * scale;
   float y = aPos.y / (window_h/100) * scale;

   gl_Position = vec4(x, y, 0, 1.0);
   TexCoord = aTexCoord;
}