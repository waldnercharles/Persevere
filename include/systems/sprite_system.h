#ifndef SPRITE_SYSTEM_H
#define SPRITE_SYSTEM_H

#include "std.h"
#include "ecs.h"

void sprite_system_process_begin(struct ecs *ecs, void *u_data);
void sprite_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt);
void sprite_system_process_end(struct ecs *ecs, void *u_data);

struct ecs_system_funcs sprite_system_funcs = {
    .process_begin = sprite_system_process_begin,
    .process = sprite_system_process,
    .process_end = sprite_system_process_end,
};

#endif