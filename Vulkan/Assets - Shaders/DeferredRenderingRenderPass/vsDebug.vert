#version 450

layout(location = 0) out vec2 outUV;

const vec4 pos[6] = vec4[6]
(
	vec4(-1.0, -1.0, 0.0, 1.0),
	vec4(+1.0, -1.0, 0.0, 1.0),
	vec4(+1.0, +1.0, 0.0, 1.0),
	vec4(-1.0, -1.0, 0.0, 1.0),
	vec4(+1.0, +1.0, 0.0, 1.0),
	vec4(-1.0, +1.0, 0.0, 1.0)
);

const vec2 uv[6] = vec2[6]
(
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 1.0)
);

void main()
{
	outUV = uv[gl_VertexIndex];
	gl_Position = pos[gl_VertexIndex];
}