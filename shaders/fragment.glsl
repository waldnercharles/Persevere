#version 330
out vec4 fragment_color;

in fragment_data
{
	vec3 col;
    vec2 tex;
} fs_in;

uniform sampler2D u_texture;

void main()
{
	fragment_color = texture(u_texture, fs_in.tex) * vec4(fs_in.col, 1.0);
}