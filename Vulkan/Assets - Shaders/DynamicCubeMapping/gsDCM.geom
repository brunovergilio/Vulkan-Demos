#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

layout (location = 0) in vec2 inTex[];

layout (location = 0) out vec2 outTex;

layout (binding = 0) uniform UBO 
{
	mat4 wvp[6];
} ubo;

void main() 
{
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < gl_in.length(); ++j)
		{
			gl_Layer = i;
			gl_Position = ubo.wvp[i] * gl_in[j].gl_Position;
			outTex = inTex[j];
			
			EmitVertex();
		}
		
		EndPrimitive();
	}
}