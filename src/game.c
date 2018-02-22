#include "game.h"
#include "function_pointers.h"
#include "unity.c"

void
game_register_components(struct ecs *ecs,
                         struct ecs_component_handles *components)
{
    ecs_register_component(ecs,
                           "movement_component",
                           sizeof(struct movement),
                           components->movement);

    ecs_register_component(ecs,
                           "body_component",
                           sizeof(struct body),
                           components->body);

    ecs_register_component(ecs,
                           "render_component",
                           sizeof(struct render),
                           components->render);

    ecs_register_component(ecs,
                           "light_component",
                           sizeof(struct light),
                           components->light);
}

void
game_register_systems(struct ecs *ecs,
                      struct ecs_component_handles *components,
                      struct ecs_system_handles *systems)
{
    ecs_register_system(ecs, "movement_system", systems->movement);
    ecs_watch(ecs, systems->movement, components->movement);
    ecs_watch(ecs, systems->movement, components->body);

    ecs_register_system(ecs, "render_system", systems->render);
    ecs_watch(ecs, systems->render, components->render);
    ecs_watch(ecs, systems->render, components->body);
}

void
game_start(struct engine *engine)
{
    u32 entity;
    struct ecs_component_handles components;
    struct body body = { 0 };
    struct render render = { 0 };
    struct light light = { 0 };

    u32 shader, *frag, *vert;
    struct mixer_source *chopin;
    struct renderer_texture *spritesheet;

    r32 tile_width = 1.0f / 32.0f;

    chopin = asset_get(engine->assets, "assets/chopin.ogg");
    frag = asset_get(engine->assets, "assets/shader.frag");
    vert = asset_get(engine->assets, "assets/shader.vert");
    spritesheet = asset_get(engine->assets, "assets/industrial.png");

    shader = shader_program_link(*vert, *frag);

    mixer_set_loop(chopin, 1);
    mixer_play(engine->mixer, chopin);

    game_register_components(engine->ecs, &ecs_component_handles);
    game_register_systems(engine->ecs,
                          &ecs_component_handles,
                          &ecs_system_handles);

    ecs_finalize(engine->ecs);

    components = ecs_component_handles;

    render.shader = shader;
    render.texture = spritesheet->id;

    render.uv_offset = vec2(0 * tile_width, 15 * tile_width);

    s32 i, j;
    body.size = vec2(2.0f / 16.0f, 2.0f / 16.0f);
    for (i = 0; i < 16; ++i)
    {
        for (j = 0; j < 16; ++j)
        {
            body.pos = vec2(i * body.size.x - 1.0f, j * body.size.y - 1.0f);

            ecs_create_entity(engine->ecs, &entity);
            ecs_set_component(engine->ecs, entity, components.body, &body);
            ecs_set_component(engine->ecs, entity, components.render, &render);
            ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);
        }
    }

    // lights
    body.pos = vec2(0.0f, 0.0f);
    body.size = vec2(1.0f, 1.0f);

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, components.light, &light);
    ecs_set_component(engine->ecs, entity, components.body, &body);
}

void
game_loop(struct engine *engine, r32 dt)
{
    ecs_process(engine->ecs, engine, dt);
}
