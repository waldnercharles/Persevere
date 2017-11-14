#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#include "game.h"

#include "engine.c"
#include "array.c"
#include "asset.c"
#include "bitset.c"
#include "map.c"
#include "ecs.c"
#include "file.c"
#include "vec.c"
#include "mixer.c"
#include "renderer.c"
#include "shader.c"
#include "sparse_set.c"

#include "assets/sound_asset.c"
#include "assets/shader_asset.c"
#include "assets/texture_asset.c"

#include "components/body_component.h"
#include "components/movement_component.h"
#include "components/render_component.h"

#include "systems/movement_system.c"
#include "systems/render_system.c"

void
game_init(struct game *game)
{
    init(game);

    u32 shader, *frag, *vert;
    struct mixer_source *chopin;
    struct renderer_texture *spritesheet;

    chopin = asset_get(game->assets, "assets/chopin.ogg");

    frag = asset_get(game->assets, "assets/shader.frag");
    vert = asset_get(game->assets, "assets/shader.vert");
    spritesheet = asset_get(game->assets, "assets/industrial.png");

    shader = shader_program_link(*frag, *vert);

    mixer_set_loop(chopin, 1);
    mixer_play(game->mixer, chopin);

    renderer_init(game->renderer);

    // components
    ecs_create_component(game->ecs,
                         "movement_component",
                         sizeof(struct movement),
                         &movement_component);

    ecs_create_component(game->ecs,
                         "body_component",
                         sizeof(struct body),
                         &body_component);

    ecs_create_component(game->ecs,
                         "render_component",
                         sizeof(struct render),
                         &render_component);

    // systems
    ecs_create_system(game->ecs,
                      "movement_system",
                      NULL,
                      movement_system_process,
                      NULL,
                      &movement_system);

    ecs_watch(game->ecs, movement_system, movement_component);
    ecs_watch(game->ecs, movement_system, body_component);

    ecs_create_system(game->ecs,
                      "render_system",
                      render_system_process_begin,
                      render_system_process,
                      render_system_process_end,
                      &render_system);

    ecs_watch(game->ecs, render_system, render_component);
    ecs_watch(game->ecs, render_system, body_component);

    // init
    ecs_init(game->ecs);

    u32 entity;
    struct body body = { 0 };
    struct render render = { 0 };

    body.pos = vec2(-0.5f, -0.5f);
    body.size = vec2(1.0f, 1.0f);

    render.shader = shader;
    render.texture = spritesheet->id;

    f32 tile_width = 1.0f / 32.0f;
    render.uv_offset = vec2(0 * tile_width, 15 * tile_width);

    ecs_create_entity(game->ecs, &entity);
    ecs_set_component(game->ecs, entity, body_component, &body);
    ecs_set_component(game->ecs, entity, render_component, &render);
    ecs_set_state(game->ecs, entity, ECS_STATE_ADDED);
}

void
game_loop(struct game *game, f32 dt)
{
    unused(game), unused(dt);
    ecs_process(game->ecs, game, dt);
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
