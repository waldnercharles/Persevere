#version 330

layout (location = 0) in vec2 quad;
layout (location = 1) in vec2 pos;
layout (location = 2) in vec2 size;
layout (location = 3) in vec3 color;
layout (location = 4) in float intensity;

out fragment_data
{
    vec2 pos;
    vec4 color;
} vs_out;


void main()
{
    gl_Position = vec4(quad * size + pos, 0.0, 1.0);

    vs_out.pos = quad.xy;
    vs_out.color = vec4(color, intensity);
}