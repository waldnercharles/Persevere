#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "ecs.h"

u32 movement_system;

void movement_system_process(struct ecs *ecs, void *u_data, u32 entity, f32 dt);
#endif