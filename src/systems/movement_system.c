#include "systems/movement_system.h"

#include "components.h"

void
movement_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    unused(u_data);
    struct position *position;
    struct movement *movement;
    struct components *components = ecs->component_handles;

    ecs_get_component(ecs, entity, components->position, (void **)&position);
    ecs_get_component(ecs, entity, components->movement, (void **)&movement);

    position->pos.x += movement->vel.x * dt;
    position->pos.y += movement->vel.y * dt;
    // body->theta += movement->angular_vel * dt;
}