#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "components.h"

#include "components/position_component.h"
#include "components/movement_component.h"
#include "components/quad_component.h"
#include "components/material_component.h"
#include "components/mouse_follow_component.h"
#include "components/light_component.h"
#include "components/shadowcaster_component.h"

struct components
{
    u32 position;
    u32 movement;
    u32 mouse_follow;
    u32 quad;
    u32 material;
    u32 light;
    u32 shadowcaster;
};

#define get_component_id(ecs, c)                                               \
    (*(u32 *)(((u8 *)ecs->component_handles) + offsetof(struct components, c)))

#define register_component(e, c)                                               \
    ecs_register_component(e,                                                  \
                           #c "_component",                                    \
                           sizeof(struct c),                                   \
                           &get_component_id(ecs, c));

#endif