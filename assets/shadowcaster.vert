#version 330

layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 pos;
layout(location = 2) in vec2 size;

void
main()
{
    gl_Position = vec4(vert * size + pos, 0.0, 1.0);
}