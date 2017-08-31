#include "game_core.h"

#include "nerd_bitset.c"
#include "nerd_echo.c"
#include "nerd_math.c"
#include "nerd_memory.c"
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

static void movement_system_process(struct echo *echo, uint entity, float dt)
{
    struct position *pos;
    struct velocity *vel;

    echo_entity_get_component(echo, entity, position_component, (void **)&pos);
    echo_entity_get_component(echo, entity, velocity_component, (void **)&vel);

    pos->x += vel->x * dt;
    pos->y += vel->y * dt;

    printf("%i moved to (%f, %f)\n", entity, pos->x, pos->y);
}

void game_init(struct game *game)
{
    game->echo = malloc(sizeof(struct echo));
    memset(game->echo, 0, sizeof(struct echo));

    echo_component_create(game->echo, "position", sizeof(struct position), &position_component);
    echo_component_create(game->echo, "velocity", sizeof(struct velocity), &velocity_component);

    uint movement_system;
    echo_system_create(game->echo, "movement", movement_system_process, &movement_system);
    echo_system_watch_component(game->echo, movement_system, position_component);
    echo_system_watch_component(game->echo, movement_system, velocity_component);

    echo_init(game->echo);

    struct position pos;
    struct velocity vel;

    uint e1;
    pos.x = 7.5f, pos.y = 1.0f;
    vel.x = 1.2f, vel.y = 3.14f;
    echo_entity_create(game->echo, &e1);
    echo_entity_set_component(game->echo, e1, position_component, &pos);
    echo_entity_set_component(game->echo, e1, velocity_component, &vel);
    echo_entity_set_state(game->echo, e1, ECHO_ENTITY_ADDED);
    printf("pos: (%f, %f)\n", pos.x, pos.y);

    uint e2;
    pos.x = 5.0f, pos.y = 4.0f;
    vel.x = 1.0f, vel.y = -9.81f;
    echo_entity_create(game->echo, &e2);
    echo_entity_set_component(game->echo, e2, position_component, &pos);
    echo_entity_set_component(game->echo, e2, velocity_component, &vel);
    echo_entity_set_state(game->echo, e2, ECHO_ENTITY_ADDED);
}

void game_loop(struct game *game, float dt)
{
    // echo_process(game_state->echo, dt);
}
