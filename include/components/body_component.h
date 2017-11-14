#pragma once
#include "std.h"
#include "vec.h"

struct body
{
    union vec2 pos;
    union vec2 size;
    f32 theta;
};
u32 body_component;
