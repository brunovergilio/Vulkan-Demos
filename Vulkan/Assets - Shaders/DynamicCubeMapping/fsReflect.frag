#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform Cam
{
	vec3 camPos;
} cam;

layout (binding = 2) uniform samplerCube samplerCubeMap;

void main()
{
	vec3 I = normalize(inPos - cam.camPos);
	vec3 R = reflect(I, normalize(inNormal));
	
	outColor = texture(samplerCubeMap, R);
}