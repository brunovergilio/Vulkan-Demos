#version 450

layout(location = 0) out vec4 outColor;

layout(input_attachment_index = 0, set = 0, binding = 0) uniform highp subpassInput depthInput;
layout(input_attachment_index = 1, set = 0, binding = 1) uniform mediump subpassInput diffuseInput;
layout(input_attachment_index = 2, set = 0, binding = 2) uniform mediump subpassInput normalInput;

layout (binding = 3) uniform DebugData
{
	vec4 debugValues;
} debugData;

void main()
{
	float currPixelX = gl_FragCoord.x * debugData.debugValues.x;
	
	if (currPixelX < debugData.debugValues.z)
	{
		outColor = vec4(subpassLoad(depthInput).xxx, 1.0);
	}
	else if (currPixelX < debugData.debugValues.w)
	{
		outColor = subpassLoad(diffuseInput);
	}
	else
	{
		outColor = vec4((subpassLoad(normalInput) * 2.0 - 1.0).xyz, 1.0);
	}
}