#version 330

layout(location = 0) in vec2 quad;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec2 size;
layout(location = 3) in vec2 tex;

out fragment_data
{
    vec4 color;
    vec2 quad;
}
vs_out;

void
main()
{
    gl_Position = vec4(quad * size + pos.xy, pos.z, 1.0);
    vs_out.color = vec4(1.0, 0.0, 0.0, 1.0);
    vs_out.quad = quad;
}