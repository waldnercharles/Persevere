#include "systems/movement_system.h"

#include "components/body_component.h"
#include "components/movement_component.h"

void
movement_system_process(struct ecs *ecs, void *u_data, u32 entity, f32 dt)
{
    (void)u_data;
    struct movement *movement;
    struct body *body;

    ecs_get_component(ecs, entity, body_component, (void **)&body);
    ecs_get_component(ecs, entity, movement_component, (void **)&movement);

    body->pos.x += movement->vel.x * dt;
    body->pos.y += movement->vel.y * dt;
    body->theta += movement->angular_vel * dt;
}