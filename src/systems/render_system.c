#include "std.h"
#include "engine.h"
#include "array.h"
#include "vec.h"
#include "renderer.h"
#include "renderers/sprite_renderer.h"

#include "ecs_handles.h"

#include "systems/render_system.h"

#include "components/body_component.h"
#include "components/render_component.h"

void
render_system_process_begin(struct ecs *ecs, void *u_data)
{
    struct engine *engine;
    struct sprite_renderer *sprite_renderer;
    unused(ecs);

    engine = u_data;
    sprite_renderer = engine->renderer->sprite_renderer;

    array__len(sprite_renderer->sprites) = 0;
    array__len(sprite_renderer->shaders) = 0;
    array__len(sprite_renderer->textures) = 0;
}

void
render_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    u32 body_component, render_component;
    struct body *body;
    struct render *render;

    struct sprite_vertex sprite;

    struct engine *engine;
    struct sprite_renderer *sprite_renderer;
    unused(dt);

    engine = u_data;
    sprite_renderer = engine->renderer->sprite_renderer;

    body_component = ecs_component_handles.body;
    render_component = ecs_component_handles.render;

    ecs_get_component(ecs, entity, body_component, (void **)&body);
    ecs_get_component(ecs, entity, render_component, (void **)&render);

    sprite.pos = body->pos;
    sprite.size = body->size;
    // sprite.theta = body->theta;
    sprite.uv = render->uv_offset;

    array_push(sprite_renderer->sprites, sprite);
    array_push(sprite_renderer->shaders, render->shader);
    array_push(sprite_renderer->textures, render->texture);
}

void
render_system_process_end(struct ecs *ecs, void *u_data)
{
    struct engine *engine;
    engine = u_data;

    renderer_render(engine->renderer);

    (void)ecs;
}