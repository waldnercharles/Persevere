#include "nerd.h"
#include "nerd_array.h"
#include "nerd_math.h"
#include "nerd_renderer.h"

#include "systems/render_system.h"

#include "components/body_component.h"
#include "components/render_component.h"

void
render_system_process_begin(struct echo *echo, void *u_data)
{
    struct game *game;
    unused(echo);

    game = u_data;
    array__len(game->renderer->sprites) = 0;
    array__len(game->renderer->states) = 0;
}

void
render_system_process(struct echo *echo, void *u_data, u32 entity, f32 dt)
{
    struct game *game;
    struct body *body;
    struct render *render;

    struct renderer_sprite sprite;
    struct renderer_state state;
    unused(dt);

    game = u_data;

    echo_get_component(echo, entity, body_component, (void **)&body);
    echo_get_component(echo, entity, render_component, (void **)&render);

    sprite.pos = body->pos;
    sprite.size = body->size;
    sprite.theta = body->theta;
    sprite.uv = render->uv_offset;

    state.shader = render->shader;
    state.texture = render->texture;

    array_push(game->renderer->sprites, sprite);
    array_push(game->renderer->states, state);
}

void
render_system_process_end(struct echo *echo, void *u_data)
{
    static u32 *temp = NULL;
    struct game *game;
    u32 len, i;

    game = u_data;

    renderer_render(game->renderer);

    len = array_count(game->renderer->sprites);

    array_set_cap(temp, len);
    for (i = 0; i < len; ++i)
    {
        temp[i] = i;
    }

    // TODO: Sort temp by sprites[temp] and states[temp]
    // TODO: Re-order sprites and states according to temp order.

    (void)echo;
}