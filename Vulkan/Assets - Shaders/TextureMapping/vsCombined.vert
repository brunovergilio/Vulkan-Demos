#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;

layout (location = 0) out vec2 outTexCoords;

layout (binding = 0) uniform UBO 
{
	mat4 wvp;
} ubo;

void main() 
{
	outTexCoords = inTexCoords;
	gl_Position = ubo.wvp * vec4(inPos.xyz, 1.0);
}