#ifndef BODY_COMPONENT_H
#define BODY_COMPONENT_H

#include "std.h"
#include "vec.h"

struct body
{
    union vec2 pos;
    union vec2 size;
    r32 theta;
};

#endif