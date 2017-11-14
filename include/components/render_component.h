#pragma once
#include "std.h"
#include "vec.h"

struct render
{
    u32 shader;
    u32 texture;
    union vec2 uv_offset;
};
u32 render_component;
