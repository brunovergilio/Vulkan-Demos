#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform UBO 
{
	mat4 wvp;
} ubo;

void main() 
{
	outColor = inColor;
	gl_Position = ubo.wvp * vec4(inPos.xyz, 1.0);
}