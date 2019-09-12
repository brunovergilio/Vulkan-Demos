#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;

layout (location = 0) out vec2 outTexCoords;
layout (location = 1) out uint outTexIndex;

layout (binding = 0) uniform UBO 
{
	mat4 wvp;
	uint textureIndex;
} ubo;

void main() 
{
	outTexCoords = inTexCoords;
	outTexIndex = ubo.textureIndex;
	
	gl_Position = ubo.wvp * vec4(inPos.xyz, 1.0);
}