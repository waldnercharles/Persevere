#ifndef RENDER_COMPONENT_H
#define RENDER_COMPONENT_H

#include "std.h"
#include "vec.h"

struct render
{
    u32 shader;
    u32 texture;
    union vec2 uv_offset;
};

#endif