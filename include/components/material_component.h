#ifndef MATERIAL_COMPONENT_H
#define MATERIAL_COMPONENT_H

#include "std.h"
#include "vec.h"

struct material
{
    u32 shader;
    u32 texture;
    v2 uv_offset;
};

#endif