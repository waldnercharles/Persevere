#version 330

layout(location = 0) in vec2 quad;
layout(location = 1) in vec3 pos;
layout(location = 2) in vec2 size;
layout(location = 3) in vec2 tex;
// layout (location = 4) in float theta;

// TODO: Model View Projection

out fragment_data
{
    vec4 color;
    vec2 tex;
}
vs_out;

// vec2
// rotate(vec2 v, float a)
// {
//     float s = sin(a);
//     float c = cos(a);
//     mat2 m = mat2(c, -s, s, c);
//     return m * v;
// }

void
main()
{
    gl_Position = vec4(quad * size + pos.xy, 0.0, 1.0);
    vs_out.color = vec4(1.0, 1.0, 1.0, 1.0);
    vs_out.tex = ((quad + 0.5) / 32.0) + tex;
}