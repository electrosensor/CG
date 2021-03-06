#version 330

in  vec2 texCoord;
out vec4 colour;

uniform sampler2D textureSampler;


struct DirectionalLight 
{
	vec3 colour;
	float ambientIntensity;
};


uniform DirectionalLight directionalLight;

void main() 
{ 
	vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

    colour = texture(textureSampler, texCoord) * ambientColour;
} 

