#include "std.h"
#include "engine.h"
#include "array.h"
#include "vec.h"
#include "renderer.h"

#include "systems/render_system.h"

#include "components/body_component.h"
#include "components/render_component.h"

void
render_system_process_begin(struct ecs *ecs, void *u_data)
{
    struct engine *engine;
    unused(ecs);

    engine = u_data;
    array__len(engine->renderer->sprites) = 0;
    array__len(engine->renderer->states) = 0;
}

void
render_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    u32 body_component, render_component;
    struct engine *engine;
    struct body *body;
    struct render *render;

    struct renderer_sprite sprite;
    struct renderer_state state;
    unused(dt);

    engine = u_data;
    body_component = engine->component_handles.body;
    render_component = engine->component_handles.render;

    ecs_get_component(ecs, entity, body_component, (void **)&body);
    ecs_get_component(ecs, entity, render_component, (void **)&render);

    sprite.pos = body->pos;
    sprite.size = body->size;
    sprite.theta = body->theta;
    sprite.uv = render->uv_offset;

    state.shader = render->shader;
    state.texture = render->texture;

    array_push(engine->renderer->sprites, sprite);
    array_push(engine->renderer->states, state);
}

void
render_system_process_end(struct ecs *ecs, void *u_data)
{
    static u32 *temp = NULL;
    struct engine *engine;
    u32 len, i;

    if (temp == NULL)
    {
        array_init(temp, ecs->allocator);
    }

    engine = u_data;

    renderer_render(engine->renderer);

    len = array_count(engine->renderer->sprites);

    array_set_cap(temp, len);
    for (i = 0; i < len; ++i)
    {
        temp[i] = i;
    }

    // TODO: Sort temp by sprites[temp] and states[temp]
    // TODO: Re-order sprites and states according to temp order.

    (void)ecs;
}