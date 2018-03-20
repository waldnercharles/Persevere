#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "std.h"
#include "vec.h"

struct light
{
    r32 depth;
    r32 physical_radius;
    r32 light_radius;

    v3 color;
    r32 intensity;
};

struct light
light(v3 color, r32 intensity, r32 light_radius, r32 physical_radius, r32 depth)
{
    struct light light = { .color = color,
                           .intensity = intensity,
                           .light_radius = light_radius,
                           .physical_radius = physical_radius,
                           .depth = depth };

    return light;
}

#endif