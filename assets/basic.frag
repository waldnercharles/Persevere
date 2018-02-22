#version 330
out vec3 fragment_color;

in fragment_data
{
	vec3 color;
} fs_in;

void main()
{
	fragment_color = fs_in.color;
}