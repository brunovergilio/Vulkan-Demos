#version 450

layout (location = 0) in vec2 inTexCoords;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D samplerTexture;

void main()
{
	outColor = texture(samplerTexture, inTexCoords);
}