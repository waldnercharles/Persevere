#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 col;
out fragment_data
{
	vec4 col;
} vs_out;

void main()
{
	gl_Position = vec4(pos, 1.0);
	vs_out.col = col;
}