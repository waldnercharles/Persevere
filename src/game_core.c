#include "game_core.h"

#include "nerd.h"
#include "nerd_bitset.c"
#include "nerd_echo.c"
#include "nerd_math.c"
#include "nerd_memory.h"
#include "nerd_sparse_set.c"

struct position
{
    float x, y;
};
static uint position_component;

struct velocity
{
    float x, y;
};
static uint velocity_component;

static void
movement_system_process(struct echo *echo, uint entity, float dt)
{
    struct position *pos;
    struct velocity *vel;

    pos = echo_entity_get_component(echo, entity, position_component);
    vel = echo_entity_get_component(echo, entity, velocity_component);

    pos->x += vel->x * dt;
    pos->y += vel->y * dt;

    printf("%i moved to (%f, %f)\n", entity, pos->x, pos->y);
}

void
game_init(struct game *game)
{
    uint e1, e2;
    uint movement_system;
    struct position pos;
    struct velocity vel;

    game->echo = malloc(sizeof(struct echo));
    memset(game->echo, 0, sizeof(struct echo));

    position_component =
        echo_component_create(game->echo, "position", sizeof(struct position));

    velocity_component =
        echo_component_create(game->echo, "velocity", sizeof(struct velocity));

    movement_system =
        echo_system_create(game->echo, "movement", movement_system_process);

    echo_system_watch_component(game->echo,
                                movement_system,
                                position_component);

    echo_system_watch_component(game->echo,
                                movement_system,
                                velocity_component);

    echo_init(game->echo);

    pos.x = 7.5f, pos.y = 1.0f;
    vel.x = 1.2f, vel.y = 3.14f;
    e1 = echo_entity_create(game->echo);
    echo_entity_set_component(game->echo, e1, position_component, &pos);
    echo_entity_set_component(game->echo, e1, velocity_component, &vel);
    echo_entity_set_state(game->echo, e1, ECHO_ENTITY_ADDED);

    pos.x = 5.0f, pos.y = 4.0f;
    vel.x = 1.0f, vel.y = -9.81f;
    e2 = echo_entity_create(game->echo);
    echo_entity_set_component(game->echo, e2, position_component, &pos);
    echo_entity_set_component(game->echo, e2, velocity_component, &vel);
    echo_entity_set_state(game->echo, e2, ECHO_ENTITY_ADDED);
}

void
game_loop(struct game *game, float dt)
{
    (void)dt;

    uint shader_program = game->shader;

    union mat4 model, view, projection;
    model = mat4(1.0f);

    model = mat4_mul(model, mat4_translate(vec3(0.0f, 0.0f, 0.0f)));

    model = mat4_mul(model,
                     mat4_rotate(vec3(1.0f, 0.0f, 0.0f), math_radians(-65.0f)));

    model = mat4_mul(model,
                     mat4_rotate(vec3(0.0f, 0.0f, 1.0f), math_radians(30.0f)));
    view = mat4_translate(vec3(0.0f, 0.0f, -3.0f));
    projection = mat4_perspective(math_radians(45.0f), 1.0f, 0.1f, 100.0f);

    uint model_loc = glGetUniformLocation(shader_program, "model");
    uint view_loc = glGetUniformLocation(shader_program, "view");
    uint proj_loc = glGetUniformLocation(shader_program, "projection");

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model.m[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view.m[0][0]);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection.m[0][0]);

    glUseProgram(shader_program);

    glBindVertexArray(game->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // echo_process(game_state->echo, dt);
}
