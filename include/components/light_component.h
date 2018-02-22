#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "std.h"
#include "vec.h"

struct light
{
    v2 pos;
    r32 depth;

    r32 physical_radius;
    r32 light_radius;

    v3 color;
    r32 intensity;
};

#endif