#include "systems/movement_system.h"

#include "components/body_component.h"
#include "components/movement_component.h"

void
movement_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    struct engine *engine;
    struct component_handles components;
    struct movement *movement;
    struct body *body;

    engine = u_data;
    components = engine->component_handles;

    ecs_get_component(ecs, entity, components.body, (void **)&body);
    ecs_get_component(ecs, entity, components.movement, (void **)&movement);

    body->pos.x += movement->vel.x * dt;
    body->pos.y += movement->vel.y * dt;
    body->theta += movement->angular_vel * dt;
}