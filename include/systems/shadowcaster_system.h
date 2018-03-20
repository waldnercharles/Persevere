
#ifndef SHADOWCASTER_SYSTEM_H
#define SHADOWCASTER_SYSTEM_H

#include "std.h"
#include "ecs.h"

void shadowcaster_system_process_begin(struct ecs *ecs, void *u_data);

void shadowcaster_system_process(struct ecs *ecs,
                                 void *u_data,
                                 u32 entity,
                                 r32 dt);

#endif