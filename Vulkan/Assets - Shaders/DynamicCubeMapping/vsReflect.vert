#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec3 outPos;
layout (location = 1) out vec3 outNormal;

layout (binding = 0) uniform UBO 
{
	mat4 w;
	mat4 vp;
} ubo;

void main() 
{
	outNormal = mat3(ubo.w) * inNormal;
	outPos = (ubo.w * vec4(inPos, 1.0)).xyz;
	gl_Position = ubo.vp * ubo.w * vec4(inPos, 1.0);
}