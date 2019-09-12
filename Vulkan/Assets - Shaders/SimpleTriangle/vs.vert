#version 450

vec2 vertices[] = 
{
	vec2(-0.7f,  0.7f),
	vec2( 0.0f, -0.7f),
	vec2( 0.7f,  0.7f)
};

void main()
{
	gl_Position = vec4(vertices[gl_VertexIndex], 0.0f, 1.0f);
}