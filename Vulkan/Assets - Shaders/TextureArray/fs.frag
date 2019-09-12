#version 450

layout (location = 0) in vec2 inTexCoords;
layout (location = 1) flat in uint inTexIndex;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2DArray samplerTextureArray;

void main()
{
	outColor = texture(samplerTextureArray, vec3(inTexCoords, inTexIndex));
}