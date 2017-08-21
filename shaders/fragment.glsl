#version 330
out vec4 fragment_color;

in fragment_data
{
	vec4 col;
} fs_in;

void main()
{
	fragment_color = fs_in.col;
}