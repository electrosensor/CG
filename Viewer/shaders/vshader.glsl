#version 330

layout (location = 0) in  vec3 vPosition;
layout (location = 1) in  vec2 vTexCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 texCoord;

void main()
{
    gl_Position = Projection * View * Model * vec4(vPosition,1);
    texCoord = vTexCoord;
}
