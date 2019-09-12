#version 450

layout (location = 0) in vec3 inPos;

layout (location = 0) out vec3 outTexCoords;

layout (binding = 0) uniform UBO
{
	mat4 wvp;
} ubo;

void main() 
{
	gl_Position = (ubo.wvp * vec4(inPos.xyz, 1.0)).xyww;
	outTexCoords = inPos;
}