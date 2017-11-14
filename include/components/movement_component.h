#pragma once
#include "nerd_engine.h"
#include "nerd_math.h"

struct movement
{
    union vec2 vel;
    f32 angular_vel;
};
u32 movement_component;