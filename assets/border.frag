#version 330
out vec4 fragment_color;

in fragment_data
{
    vec4 color;
    vec2 quad;
}
fs_in;

bool isBorder(vec2 point, float bounds)
{
    return
    (
        fs_in.quad.x < -bounds ||
        fs_in.quad.x > bounds ||
        fs_in.quad.y < -bounds ||
        fs_in.quad.y > bounds
    );
}

void
main()
{
    if (isBorder(fs_in.quad, 0.45))
    {
        fragment_color = fs_in.color;
    }
    else
    {
        fragment_color = vec4(0.0, 0.0, 0.0, 0.0);
    }
}