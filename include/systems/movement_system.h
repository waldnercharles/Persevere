#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "ecs.h"

void movement_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt);

#endif