#pragma once
#include "std.h"
#include "vec.h"

struct movement
{
    union vec2 vel;
    f32 angular_vel;
};
u32 movement_component;