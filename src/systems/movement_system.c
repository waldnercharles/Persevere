#include "systems/movement_system.h"

#include "components.h"

void
movement_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    struct components *components = ecs->component_handles;

    (void)u_data;
    v2 *position;
    struct movement *movement;

    ecs_get_component(ecs, entity, components->position, (void **)&position);
    ecs_get_component(ecs, entity, components->movement, (void **)&movement);

    position->x += movement->vel.x * dt;
    position->y += movement->vel.y * dt;
    // body->theta += movement->angular_vel * dt;
}