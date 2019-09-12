#version 450

layout (location = 0) in vec3 inTexCoords;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform samplerCube samplerCubeMap;

void main()
{
	outColor = texture(samplerCubeMap, inTexCoords);
}