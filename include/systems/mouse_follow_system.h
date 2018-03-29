#ifndef MOUSE_HOVER_SYSTEM_H
#define MOUSE_HOVER_SYSTEM_H

#include "std.h"
#include "ecs.h"

void mouse_follow_system_process(struct ecs *ecs,
                                 void *u_data,
                                 u32 entity,
                                 r32 dt);

struct ecs_system_funcs mouse_follow_system_funcs = {
    .process_begin = NULL,
    .process = mouse_follow_system_process,
    .process_end = NULL,
};

#endif