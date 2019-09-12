#version 450

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D depthInput;
layout(set = 0, binding = 1) uniform sampler2D diffuseInput;
layout(set = 0, binding = 2) uniform sampler2D normalInput;

layout (binding = 3) uniform DebugData
{
	vec4 debugValues;
} debugData;

void main()
{
	float currPixelX = gl_FragCoord.x * debugData.debugValues.x;
	
	if (currPixelX < debugData.debugValues.z)
	{
		outColor = vec4(texture(depthInput, inUV).xxx, 1.0);
	}
	else if (currPixelX < debugData.debugValues.w)
	{
		outColor = texture(diffuseInput, inUV);
	}
	else
	{
		outColor = vec4((texture(normalInput, inUV) * 2.0 - 1.0).xyz, 1.0);
	}
}