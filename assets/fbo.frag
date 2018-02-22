#version 330 core
out vec4 out_color;

in vec2 frag_uv;

uniform sampler2D frag_texture;

void main()
{
    out_color = texture(frag_texture, frag_uv);
}