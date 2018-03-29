#include "systems/mouse_follow_system.h"
#include "engine.h"

void
mouse_follow_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    unused(dt);

    r32 grid_width_x, grid_width_y;

    struct position *position;
    struct mouse_follow *mouse_follow;
    struct engine *engine;
    struct components *components;

    v2 window_size;
    v2 mouse_pos;

    engine = u_data;
    components = ecs->component_handles;

    ecs_get_component(ecs, entity, components->position, (void **)&position);
    ecs_get_component(ecs,
                      entity,
                      components->mouse_follow,
                      (void **)&mouse_follow);

    window_size =
        vec2(engine->platform->window_size_x, engine->platform->window_size_y);

    mouse_pos =
        vec2(engine->platform->mouse_pos_x, engine->platform->mouse_pos_y);

    if (mouse_follow->snap_num_tiles_x > 0 ||
        mouse_follow->snap_num_tiles_y > 0)
    {
        grid_width_x = window_size.x / mouse_follow->snap_num_tiles_x;
        grid_width_y = window_size.y / mouse_follow->snap_num_tiles_y;

        mouse_pos.x = floorf(mouse_pos.x / grid_width_x) * grid_width_x;
        mouse_pos.y = floorf(mouse_pos.y / grid_width_y) * grid_width_y;

        mouse_pos.x += grid_width_x / 2.0f;
        mouse_pos.y += grid_width_y / 2.0f;
    }

    position->pos.x = mouse_pos.x / window_size.x * 2.0f - 1.0f;
    position->pos.y = -(mouse_pos.y / window_size.y * 2.0f - 1.0f);
}
