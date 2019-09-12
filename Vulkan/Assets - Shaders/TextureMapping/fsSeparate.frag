#version 450

layout (location = 0) in vec2 inTexCoords;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform texture2D samplerTexture;
layout (binding = 2) uniform sampler samplerObj;

void main()
{
	outColor = texture(sampler2D(samplerTexture, samplerObj), inTexCoords);
}