#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D atlas;
uniform sampler2D foliage_mask;

void main()
{
    vec3 tint = vec3(0.3, 0.7, 0.3);
    vec4 foliage_mask = texture(foliage_mask, TexCoord);
    float mean = (tint.r+tint.g+tint.b)/3;
    float sat = (abs(mean-tint.r) + abs(mean-tint.g) + abs(tint.b))/(mean*3);
    vec3 gray = vec3(0.2989*tint.r, 0.5870*tint.g, 0.1140*tint.b);
    float br = foliage_mask.r-0.5;
    vec4 foliage = vec4(mix(tint * foliage_mask.rgb, gray, (foliage_mask.r)/4), 0);

    vec4 final = mix(texture(atlas, TexCoord), foliage, foliage_mask.a);

    FragColor = final;
}  
