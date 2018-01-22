#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "game.h"

#include "allocators/allocator.c"
#include "array.c"
#include "bitset.c"
#include "ecs.c"
#include "engine.c"
#include "file.c"
#include "map.c"
#include "mixer.c"
#include "renderer.c"
#include "shader.c"
#include "sparse_set.c"
#include "vec.c"

#include "asset.c"
#include "assets/texture_asset.c"
#include "assets/shader_asset.c"
#include "assets/sound_asset.c"

#include "components/body_component.h"
#include "components/movement_component.h"
#include "components/render_component.h"

#include "systems/movement_system.c"
#include "systems/render_system.c"

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

    body.pos = vec2(-0.5f, -0.5f);
    body.size = vec2(1.0f, 1.0f);

    render.shader = shader;
    render.texture = spritesheet->id;

    r32 tile_width = 1.0f / 32.0f;
    render.uv_offset = vec2(0 * tile_width, 15 * tile_width);

    ecs_create_entity(engine->ecs, &entity);
    ecs_set_component(engine->ecs, entity, components.body, &body);
    ecs_set_component(engine->ecs, entity, components.render, &render);
    ecs_set_state(engine->ecs, entity, ECS_STATE_ADDED);
}

void
game_loop(struct engine *engine, r32 dt)
{
    ecs_process(engine->ecs, engine, dt);
    // union mat4 model, view, projection;
    // u32 shader_program;
    // (void)dt;

    // shader_program = game->shader;

    // model = mat4(1.0f);

    // model = mat4_mul(model, mat4_translate(vec3(0.0f, 0.0f, 0.0f)));

    // model = mat4_mul(model,
    //                  mat4_rotate(vec3(1.0f, 0.0f, 0.0f),
    //                  math_radians(-65.0f)));

    // model = mat4_mul(model,
    //                  mat4_rotate(vec3(0.0f, 0.0f, 1.0f),
    //                  math_radians(60.0f)));

    // view = mat4_translate(vec3(0.0f, 0.0f, -3.0f));

    // projection = mat4_perspective(math_radians(45.0f), 1.0f, 0.1f, 100.0f);

    // u32 model_loc = glGetUniformLocation(shader_program, "model");
    // u32 view_loc = glGetUniformLocation(shader_program, "view");
    // u32 proj_loc = glGetUniformLocation(shader_program, "projection");

    // glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model.m[0][0]);
    // glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view.m[0][0]);
    // glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection.m[0][0]);

    // glUseProgram(shader_program);

    // glBindVertexArray(game->VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 36);

    // ecs_process(game_state->ecs, dt);
}
