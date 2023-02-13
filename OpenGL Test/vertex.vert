#version 460 core
layout (location = 0) in vec4 coords;
layout (location = 1) in float tile;

vec2 aPos = coords.xy;
vec2 aTex = coords.zw;

out vec2 TexCoord;

uniform vec2 scale;


void main()
{
   gl_Position = vec4(aPos * scale, 0, 1.0);

   TexCoord = vec2(1.0, 0.0);
}