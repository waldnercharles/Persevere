#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "ecs.h"
#include "array.h"

#include "systems/movement_system.h"
#include "systems/mouse_follow_system.h"
#include "systems/sprite_system.h"
#include "systems/light_system.h"
#include "systems/shadowcaster_system.h"

struct systems
{
    u32 movement;
    u32 sprite;
    u32 light;
    u32 shadowcaster;
    u32 mouse_follow;
};

#define get_system_id(ecs, s)                                                  \
    (*(u32 *)(((u8 *)ecs->system_handles) + offsetof(struct systems, s)))

#define register_system(ecs, s, f)                                             \
    ecs_register_system(ecs, #s "_system", f, &get_system_id(ecs, s))

void
ecs_bind_system_funcs(struct ecs *ecs,
                      u32 system_id,
                      struct ecs_system_funcs *funcs)
{
    struct ecs_system *system;

    if (funcs == NULL)
    {
        funcs = &(struct ecs_system_funcs){};
    }

    system = array_get(ecs->systems, system_id);
    system->process_begin = funcs->process_begin;
    system->process = funcs->process;
    system->process_end = funcs->process_end;
}

#define bind_system_funcs(ecs, s)                                              \
    ecs_bind_system_funcs(ecs, get_system_id(ecs, s), &(s##_system_funcs))

#endif
