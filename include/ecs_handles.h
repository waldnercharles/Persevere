#ifndef ECS_HANDLES_H
#define ECS_HANDLES_H
#include "ecs.h"

#include "systems/movement_system.h"
#include "systems/render_system.h"


struct ecs_component_handles
{
    u32 body;
    u32 movement;
    u32 render;
    u32 spritesheet;
    u32 light;
};

struct ecs_system_handles
{
    u32 movement;
    u32 render;
};

struct ecs_system_functions
{
    void (*process_begin)(struct ecs *ecs, void *u_data);
    void (*process)(struct ecs *ecs, void *u_data, u32 entity, r32 dt);
    void (*process_end)(struct ecs *ecs, void *u_data);
};

static struct ecs_component_handles ecs_component_handles =
{
    .body = 0,
    .movement = 1,
    .render = 2,
    .spritesheet = 3,
    .light = 4,
};


static struct ecs_system_handles ecs_system_handles =
{
    .movement = 0,
    .render = 1,
};

static struct ecs_system_functions ecs_system_functions[] =
{
    // movement system
    {
        .process_begin = NULL,
        .process = movement_system_process,
        .process_end = NULL
    },
    // render system
    {
        .process_begin = render_system_process_begin,
        .process = render_system_process,
        .process_end = render_system_process_end
    },
};

#endif
