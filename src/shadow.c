#include "vec.h"
#include "components/light_component.h"

v2
get_outer_vector(struct light light, v2 pos, s32 direction)
{
    bool invert;
    v2 center_vec, perp_vec;
    r32 theta;

    center_vec = vec2_sub(light.pos, pos);

    // Calculate perpendicular
    invert = light.pos.x < pos.x;
    perp_vec = vec2_norm(center_vec);
    theta = -MATH_PI / 2.0f;

    if (direction == 1)
    {
        if (invert)
        {
        }
        perp_vec = vec2_mul(perp_vec, -light.physical_radius);
        perp_vec = vec2_rotate(perp_vec)
    }
}