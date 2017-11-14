#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H
#include "std.h"

struct ecs;

u32 render_system;
void render_system_process_begin(struct ecs *ecs, void *u_data);
void render_system_process(struct ecs *ecs, void *u_data, u32 entity, f32 dt);
void render_system_process_end(struct ecs *ecs, void *u_data);
#endif