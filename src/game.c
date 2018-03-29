#include "unity.c"

#include "game.h"
#include "components.h"
#include "systems.h"

void
game_register_components(struct ecs *ecs)
{
    register_component(ecs, position);
    register_component(ecs, movement);
    register_component(ecs, quad);
    register_component(ecs, material);
    register_component(ecs, light);
    register_component(ecs, shadowcaster);
    register_component(ecs, mouse_follow);
}

void
game_register_systems(struct ecs *ecs)
{
    struct systems *systems = ecs->system_handles;
    struct components *components = ecs->component_handles;

    register_system(ecs, movement, movement_system_funcs);
    ecs_watch(ecs, systems->movement, components->movement);
    ecs_watch(ecs, systems->movement, components->position);

    register_system(ecs, mouse_follow, mouse_follow_system_funcs);
    ecs_watch(ecs, systems->mouse_follow, components->position);
    ecs_watch(ecs, systems->mouse_follow, components->mouse_follow);

    register_system(ecs, sprite, sprite_system_funcs);
    ecs_watch(ecs, systems->sprite, components->material);
    ecs_watch(ecs, systems->sprite, components->quad);
    ecs_watch(ecs, systems->sprite, components->position);

    register_system(ecs, light, light_system_funcs);
    ecs_watch(ecs, systems->light, components->position);
    ecs_watch(ecs, systems->light, components->light);

    register_system(ecs, shadowcaster, shadowcaster_system_funcs);
    ecs_watch(ecs, systems->shadowcaster, components->position);
    ecs_watch(ecs, systems->shadowcaster, components->shadowcaster);
}

void
game_start(struct engine *engine)
{
    struct components *components = engine->ecs->component_handles;

    u32 entity;
    struct position position;
    struct quad quad;
    struct material material;
    struct light light;
    struct shadowcaster shadowcaster;
    struct mouse_follow mouse_follow;
    // struct light light = { 0 };

    u32 shader, *frag, *vert;
    u32 border_shader, *border_frag, *border_vert;
    struct mixer_source *chopin;
    struct renderer_texture *spritesheet;

    r32 tile_width = 1.0f / 32.0f;

    chopin = asset_get(engine->assets, "assets/chopin.ogg");
    frag = asset_get(engine->assets, "assets/shader.frag");
    vert = asset_get(engine->assets, "assets/shader.vert");
    border_frag = asset_get(engine->assets, "assets/border.frag");
    border_vert = asset_get(engine->assets, "assets/border.vert");
    spritesheet = asset_get(engine->assets, "assets/industrial.png");

    shader = shader_program_link(*vert, *frag);
    border_shader = shader_program_link(*border_vert, *border_frag);

    mixer_set_loop(chopin, 1);
    mixer_play(engine->mixer, chopin);

    game_register_components(engine->ecs);
    game_register_systems(engine->ecs);

    ecs_finalize(engine->ecs);

    material =
        (struct material){ .shader = shader,
                           .texture = spritesheet->id,
                           .uv_offset = vec2(0 * tile_width, 15 * tile_width) };

    s32 i, j;

    quad = (struct quad){ .size = vec2(2.0f / 16.0f, 2.0f / 16.0f) };
    for (i = 0; i < 16; ++i)
    {
        for (j = 0; j < 16; ++j)
        {
            position = (struct position){
                .pos =
                    vec3(i * quad.size.x - 1.0f, j * quad.size.y - 1.0f, 0.5f),
            };

            position.pos.x += quad.size.x / 2.0f;
            position.pos.y += quad.size.y / 2.0f;

            ecs_create_entity(engine->ecs, &entity);

            ecs_set_component(engine->ecs,
                              entity,
                              components->position,
                              &position);

            ecs_set_component(engine->ecs, entity, components->quad, &quad);

            ecs_set_component(engine->ecs,
                              entity,
                              components->material,
                              &material);

            ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);
        }
    }

    // cursor
    position = (struct position){ .pos = vec3(0.0f, 0.0f, 0.0f) };
    mouse_follow =
        (struct mouse_follow){ .snap_num_tiles_x = 16, .snap_num_tiles_y = 16 };
    material =
        (struct material){ .shader = border_shader,
                           .texture = 0,
                           .uv_offset = vec2(0 * tile_width, 15 * tile_width) };

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, components->position, &position);
    ecs_set_component(engine->ecs,
                      entity,
                      components->mouse_follow,
                      &mouse_follow);
    ecs_set_component(engine->ecs, entity, components->quad, &quad);
    ecs_set_component(engine->ecs, entity, components->material, &material);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);

    position = (struct position){ .pos = vec3(0.0f, 0.0f, -0.5f) };
    mouse_follow =
        (struct mouse_follow){ .snap_num_tiles_x = 0, .snap_num_tiles_y = 0 };
    material =
        (struct material){ .shader = shader,
                           .texture = spritesheet->id,
                           .uv_offset = vec2(9 * tile_width, 21 * tile_width) };

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, components->position, &position);
    ecs_set_component(engine->ecs,
                      entity,
                      components->mouse_follow,
                      &mouse_follow);
    ecs_set_component(engine->ecs, entity, components->quad, &quad);
    ecs_set_component(engine->ecs, entity, components->material, &material);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);

    // lights
    position = (struct position){ .pos = vec3(0.0f, 0.0f, 0.0f) };
    light = (struct light){ .color = vec3(1.0f, 1.0f, 1.0f),
                            .intensity = 1.0f,
                            .light_radius = 1.0f,
                            .physical_radius = 0.05f,
                            .depth = 0.0f };

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, components->position, &position);
    ecs_set_component(engine->ecs, entity, components->light, &light);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);

    // shadowcasters
    position = (struct position){ .pos = vec3(0.5f, 0.5f, 0.0f) };
    shadowcaster = (struct shadowcaster){ .size = vec2(0.1f, 0.1f) };

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, components->position, &position);
    ecs_set_component(engine->ecs,
                      entity,
                      components->shadowcaster,
                      &shadowcaster);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);

    // body.size = vec2(1.0f, 1.0f);    ecs_create_entity(engine->ecs, &entity);
    // ecs_set_component(engine->ecs, entity, component_position_id, &position);
    // ecs_set_component(engine->ecs, entity, component_light_id, &light);

    // ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);
}

void
game_loop(struct engine *engine, r32 dt)
{
    ecs_process(engine->ecs, engine, dt);
    renderer_render(engine->renderer);
}
