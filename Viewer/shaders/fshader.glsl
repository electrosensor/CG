#version 330

in  vec2 texCoord;
out vec3 fColor;

uniform sampler2D textureSampler;

void main() 
{ 
   fColor = texture(textureSampler, texCoord).rgb;
} 

