#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2DArray terrain;

void main()
{
   FragColor = texture(terrain, vec3(TexCoord,1));
}