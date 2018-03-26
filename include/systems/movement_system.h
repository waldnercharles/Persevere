#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "ecs.h"

void movement_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt);

struct ecs_system_funcs movement_system_funcs = {
    .process_begin = NULL,
    .process = movement_system_process,
    .process_end = NULL,
};

#endif