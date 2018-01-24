#version 330

layout (location = 0) in vec2 quad;
layout (location = 1) in vec2 pos;
layout (location = 2) in vec2 size;
layout (location = 3) in vec2 tex;
layout (location = 4) in float theta;

out fragment_data
{
	vec3 col;
    vec2 tex;
} vs_out;

vec2 rotate(vec2 v, float a)
{
    float s = sin(a);
    float c = cos(a);
    mat2 m = mat2(c, -s, s, c);
    return m * v;
}

void main()
{
    vec2 rotated_quad = (rotate(quad - vec2(0.5, 0.5), theta) + vec2(0.5, 0.5)) * size;
	gl_Position = vec4((rotated_quad + pos), 0.0, 1.0);
	vs_out.col = vec3(1.0, 1.0, 1.0);
    vs_out.tex = (quad / 32.0) + tex;
}