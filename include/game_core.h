#ifndef GAME_CORE_H
#define GAME_CORE_H

#include "nerd.h"
#include "nerd_echo.h"
#include "nerd_memory.h"

#define MAX_CONTROLLERS 4

struct game_controller
{
    void *controller;
    void *haptic;
    int joystick_id;
    int is_connected;
};

struct game
{
    struct echo *echo;
    struct mixer *mixer;

    struct game_controller controllers[MAX_CONTROLLERS];

    uint audio_device;
    uint shader;

    uint VAO;
};

void game_init(struct game *game);
void game_loop(struct game *game, float dt);

#endif
