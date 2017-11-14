#include "systems/movement_system.h"

#include "components/body_component.h"
#include "components/movement_component.h"

void
movement_system_process(struct echo *echo, void *u_data, u32 entity, f32 dt)
{
    (void)u_data;
    struct movement *movement;
    struct body *body;

    echo_get_component(echo, entity, body_component, (void **)&body);
    echo_get_component(echo, entity, movement_component, (void **)&movement);

    body->pos.x += movement->vel.x * dt;
    body->pos.y += movement->vel.y * dt;
    body->theta += movement->angular_vel * dt;
}