#include "game.h"
#include "unity.c"

void
game_create_components(struct ecs *ecs, struct component_handles *components)
{
    ecs_create_component(ecs,
                         "movement_component",
                         sizeof(struct movement),
                         &components->movement);

    ecs_create_component(ecs,
                         "body_component",
                         sizeof(struct body),
                         &components->body);

    ecs_create_component(ecs,
                         "render_component",
                         sizeof(struct render),
                         &components->render);
}

void
game_create_systems(struct ecs *ecs,
                    struct component_handles *components,
                    struct system_handles *systems)
{
    ecs_create_system(ecs,
                      "movement_system",
                      NULL,
                      movement_system_process,
                      NULL,
                      &systems->movement);

    ecs_watch(ecs, systems->movement, components->movement);
    ecs_watch(ecs, systems->movement, components->body);

    ecs_create_system(ecs,
                      "render_system",
                      render_system_process_begin,
                      render_system_process,
                      render_system_process_end,
                      &systems->render);

    ecs_watch(ecs, systems->render, components->render);
    ecs_watch(ecs, systems->render, components->body);
}

void
game_start(struct engine *engine)
{
    u32 entity;
    struct component_handles components;
    struct body body = { 0 };
    struct render render = { 0 };

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

    game_create_components(engine->ecs, &engine->component_handles);
    game_create_systems(engine->ecs,
                        &engine->component_handles,
                        &engine->system_handles);

    ecs_init(engine->ecs);

    components = engine->component_handles;

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
}

void
game_loop(struct engine *engine, r32 dt)
{
    ecs_process(engine->ecs, engine, dt);
}
