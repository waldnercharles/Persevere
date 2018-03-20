#include "unity.c"

#include "game.h"
#include "components.h"
#include "systems.h"

void
game_register_components(struct ecs *ecs)
{
    ecs_register_component(ecs,
                           "position_component",
                           sizeof(v2),
                           component_position_id);

    ecs_register_component(ecs,
                           "movement_component",
                           sizeof(struct movement),
                           component_movement_id);

    ecs_register_component(ecs,
                           "quad_component",
                           sizeof(struct quad),
                           component_quad_id);

    ecs_register_component(ecs,
                           "material_component",
                           sizeof(struct material),
                           component_material_id);

    ecs_register_component(ecs,
                           "light_component",
                           sizeof(struct light),
                           component_light_id);

    ecs_register_component(ecs,
                           "shadowcaster_component",
                           sizeof(struct shadowcaster),
                           component_shadowcaster_id);
}

void
game_register_systems(struct ecs *ecs)
{
    ecs_register_system(ecs, "movement_system", system_movement_id);
    ecs_watch(ecs, system_movement_id, component_movement_id);
    ecs_watch(ecs, system_movement_id, component_position_id);

    ecs_register_system(ecs, "sprite_system", system_sprite_id);
    ecs_watch(ecs, system_sprite_id, component_material_id);
    ecs_watch(ecs, system_sprite_id, component_quad_id);
    ecs_watch(ecs, system_sprite_id, component_position_id);

    ecs_register_system(ecs, "light_system", system_light_id);
    ecs_watch(ecs, system_light_id, component_position_id);
    ecs_watch(ecs, system_light_id, component_light_id);

    ecs_register_system(ecs, "shadowcaster_system", system_shadowcaster_id);
    ecs_watch(ecs, system_shadowcaster_id, component_position_id);
    ecs_watch(ecs, system_shadowcaster_id, component_shadowcaster_id);
}

void
game_start(struct engine *engine)
{
    u32 entity;
    v2 position;
    struct quad quad;
    struct material material;
    struct light light;
    struct shadowcaster shadowcaster;
    // struct light light = { 0 };

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
            position = vec2(i * quad.size.x - 1.0f, j * quad.size.y - 1.0f);

            ecs_create_entity(engine->ecs, &entity);

            ecs_set_component(engine->ecs,
                              entity,
                              component_position_id,
                              &position);

            ecs_set_component(engine->ecs, entity, component_quad_id, &quad);

            ecs_set_component(engine->ecs,
                              entity,
                              component_material_id,
                              &material);

            ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);
        }
    }

    // lights
    position = vec2(0.0f, 0.0f);
    light = (struct light){ .color = vec3(1.0f, 1.0f, 1.0f),
                            .intensity = 1.0f,
                            .light_radius = 1.0f,
                            .physical_radius = 0.05f,
                            .depth = 0.0f };

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, component_position_id, &position);
    ecs_set_component(engine->ecs, entity, component_light_id, &light);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);

    // shadowcasters
    position = vec2(0.5f, 0.5f);
    shadowcaster = (struct shadowcaster){ .size = vec2(0.1f, 0.1f) };

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, component_position_id, &position);
    ecs_set_component(engine->ecs,
                      entity,
                      component_shadowcaster_id,
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
