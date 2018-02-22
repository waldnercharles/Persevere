#version 330 core
layout (location = 0) in vec2 vec_pos;
layout (location = 1) in vec2 vec_uv;

out vec2 frag_uv;

void main()
{
    gl_Position = vec4(vec_pos.x, vec_pos.y, 0.0, 1.0);
    frag_uv = vec_uv;
}