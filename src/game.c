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
create_tiles(struct engine *engine)
{
    struct components *c = engine->ecs->component_handles;
    struct position position;
    struct quad quad;
    struct material mat;
    u32 entity, i, j;

    u32 shader, *frag, *vert;
    struct renderer_texture *spritesheet;

    r32 tile_width = 1.0f / 32.0f;

    frag = asset_get(engine->assets, "assets/shader.frag");
    vert = asset_get(engine->assets, "assets/shader.vert");
    shader = shader_program_link(*vert, *frag);

    spritesheet = asset_get(engine->assets, "assets/industrial.png");

    quad.size = vec2(2.0f / 16.0f, 2.0f / 16.0f);
    mat.shader = shader;
    mat.texture = spritesheet->id;
    mat.uv_offset = vec2(0 * tile_width, 15 * tile_width);

    for (i = 0; i < 16; ++i)
    {
        for (j = 0; j < 16; ++j)
        {
            position.pos =
                vec3(i * quad.size.x - 1.0f, j * quad.size.y - 1.0f, 0.0f);

            position.pos.x += quad.size.x / 2.0f;
            position.pos.y += quad.size.y / 2.0f;

            ecs_create_entity(engine->ecs, &entity);

            ecs_set_component(engine->ecs, entity, c->position, &position);
            ecs_set_component(engine->ecs, entity, c->quad, &quad);
            ecs_set_component(engine->ecs, entity, c->material, &mat);

            ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);
        }
    }
}

void
create_cursor(struct engine *engine)
{
    struct components *c = engine->ecs->component_handles;
    struct position position;
    struct quad quad;
    struct material mat;
    struct mouse_follow follow;
    u32 entity;

    u32 sprite_shader, *sprite_frag, *sprite_vert;
    u32 border_shader, *border_frag, *border_vert;
    struct renderer_texture *spritesheet;

    r32 tile_width = 1.0f / 32.0f;

    // TODO: Do not link the same combination of frag and vert twice.
    sprite_frag = asset_get(engine->assets, "assets/shader.frag");
    sprite_vert = asset_get(engine->assets, "assets/shader.vert");
    sprite_shader = shader_program_link(*sprite_vert, *sprite_frag);

    border_frag = asset_get(engine->assets, "assets/border.frag");
    border_vert = asset_get(engine->assets, "assets/border.vert");
    border_shader = shader_program_link(*border_vert, *border_frag);

    spritesheet = asset_get(engine->assets, "assets/industrial.png");

    quad.size = vec2(2.0f / 16.0f, 2.0f / 16.0f);
    position.pos = vec3(0.0f, 0.0f, 1.0f);

    follow.snap_num_tiles_x = 16;
    follow.snap_num_tiles_y = 16;

    mat.shader = border_shader;
    mat.texture = 0;
    mat.uv_offset = vec2(0, 0);

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, c->position, &position);
    ecs_set_component(engine->ecs, entity, c->mouse_follow, &follow);
    ecs_set_component(engine->ecs, entity, c->quad, &quad);
    ecs_set_component(engine->ecs, entity, c->material, &mat);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);

    position.pos = vec3(0.0f, 0.0f, 2.0f);

    follow.snap_num_tiles_x = 0;
    follow.snap_num_tiles_y = 0;

    mat.shader = sprite_shader;
    mat.texture = spritesheet->id;
    mat.uv_offset = vec2(9 * tile_width, 21 * tile_width);

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, c->position, &position);
    ecs_set_component(engine->ecs, entity, c->mouse_follow, &follow);
    ecs_set_component(engine->ecs, entity, c->quad, &quad);
    ecs_set_component(engine->ecs, entity, c->material, &mat);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);
}

void
game_start(struct engine *engine)
{
    // struct mixer_source *chopin;

    // chopin = asset_get(engine->assets, "assets/chopin.ogg");

    // mixer_set_loop(chopin, 1);
    // mixer_play(engine->mixer, chopin);

    game_register_components(engine->ecs);
    game_register_systems(engine->ecs);

    ecs_finalize(engine->ecs);

    create_tiles(engine);
    create_cursor(engine);
}

void
game_loop(struct engine *engine, r32 dt)
{
    ecs_process(engine->ecs, engine, dt);
    renderer_render(engine->renderer);
}
