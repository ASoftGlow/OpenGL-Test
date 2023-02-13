#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2DArray atlas;

void main()
{
   FragColor = texture(atlas, vec3(TexCoord,1));
}