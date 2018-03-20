#include "systems/movement_system.h"

#include "components.h"

void
movement_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    (void)u_data;
    v2 *position;
    struct movement *movement;

    ecs_get_component(ecs, entity, component_position_id, (void **)&position);
    ecs_get_component(ecs, entity, component_movement_id, (void **)&movement);

    position->x += movement->vel.x * dt;
    position->y += movement->vel.y * dt;
    // body->theta += movement->angular_vel * dt;
}