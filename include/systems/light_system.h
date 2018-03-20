#ifndef LIGHT_SYSTEM_H
#define LIGHT_SYSTEM_H

#include "std.h"
#include "ecs.h"

void light_system_process_begin(struct ecs *ecs, void *u_data);
void light_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt);

#endif