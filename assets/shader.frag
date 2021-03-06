#version 330
out vec4 fragment_color;

in fragment_data
{
    vec4 color;
    vec2 tex;
}
fs_in;

uniform sampler2D sprite_sheet;

void
main()
{
    fragment_color = texture(sprite_sheet, fs_in.tex) * fs_in.color;
}