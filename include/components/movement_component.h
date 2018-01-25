#ifndef MOVEMENT_COMPONENT_H
#define MOVEMENT_COMPONENT_H

#include "std.h"
#include "vec.h"

struct movement
{
    v2 vel;
    r32 angular_vel;
};

#endif