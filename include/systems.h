#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "systems/movement_system.h"
#include "systems/sprite_system.h"
#include "systems/light_system.h"
#include "systems/shadowcaster_system.h"

#define system_movement_id 0
#define system_sprite_id 1
#define system_light_id 2
#define system_shadowcaster_id 3

struct system_functions
{
    void (*process_begin)(struct ecs *ecs, void *u_data);
    void (*process)(struct ecs *ecs, void *u_data, u32 entity, r32 dt);
    void (*process_end)(struct ecs *ecs, void *u_data);
};

static struct system_functions system_functions[] = {
    // movement system
    { .process_begin = NULL,
      .process = movement_system_process,
      .process_end = NULL },
    // render system
    { .process_begin = sprite_system_process_begin,
      .process = sprite_system_process,
      .process_end = NULL },
    // light system
    { .process_begin = light_system_process_begin,
      .process = light_system_process,
      .process_end = NULL },
    // shadowcaster system
    { .process_begin = shadowcaster_system_process_begin,
      .process = shadowcaster_system_process,
      .process_end = NULL }
};

#endif
