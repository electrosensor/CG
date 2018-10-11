#version 330

in  vec3 texCoord;
out vec4 fColor;

uniform sampler2D texture;

void main() 
{ 
   fColor = vec4(texCoord,1);
} 

