#version 330
out vec3 fragment_color;

in fragment_data
{
    vec2 pos;
	vec3 color;
} fs_in;

void main()
{
    vec2 pos = fs_in.pos;
    vec3 color = fs_in.color;

    float t = 1.0 - sqrt(pos.x*pos.x + pos.y*pos.y);
    // t = 1.0 / (1.0 + exp(-(t*12.0 - 6.0)));
	// fragment_color = vec4(color, intensity) * t;
    // fragment_color = vec4(color, intensity);
    fragment_color = color * t;
}