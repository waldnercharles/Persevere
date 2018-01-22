#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "std.h"
#include "ecs.h"

void render_system_process_begin(struct ecs *ecs, void *u_data);
void render_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt);
void render_system_process_end(struct ecs *ecs, void *u_data);

#endif