#version 150

in  vec3 vTexCoord;
in  vec3 vPosition;

uniform mat4 MVP;

out vec3 texCoord;

void main()
{
    gl_Position = MVP * vec4(vPosition,1);
    texCoord = vTexCoord;
}
