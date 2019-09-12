#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;

layout (binding = 1) uniform sampler2D samplerTexture;

void main()
{
	outColor = texture(samplerTexture, inUV);
	outNormal = vec4(normalize(inNormal) * 0.5 + 0.5, 1.0);
}