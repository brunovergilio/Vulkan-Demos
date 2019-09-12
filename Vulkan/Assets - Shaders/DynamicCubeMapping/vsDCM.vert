#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTex;

layout (location = 0) out vec2 outTex;


void main() 
{
	outTex = inTex;
	gl_Position = vec4(inPos, 1.0);
}