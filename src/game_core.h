#ifndef GAME_CORE_H
#define GAME_CORE_H

#include "nerd.h"
#include "nerd_echo.h"
#include "nerd_memory.h"

#define MAX_CONTROLLERS 4

struct game_controller
{
    SDL_GameController *controller;
    SDL_Haptic *haptic;
    SDL_JoystickID joystick_id;
    int is_connected;
};

struct game
{
    struct echo *echo;
    struct mixer *mixer;

    SDL_Surface *pixel_buffer;
    SDL_AudioDeviceID audio_device;
    struct game_controller controllers[MAX_CONTROLLERS];

    int x_offset;
    int y_offset;
};

void game_init(struct game *game);
void game_loop(struct game *game, float dt);

#endif
