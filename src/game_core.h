#ifndef GAME_CORE_H
#define GAME_CORE_H

#include "SDL.h"

#define MAX_CONTROLLERS 4


typedef struct
{
    SDL_GameController* controller;
    SDL_Haptic* haptic;
    SDL_JoystickID joystick_id;
    int is_connected;
} game_controller;


typedef struct
{
    SDL_Surface* pixel_buffer;
    SDL_AudioDeviceID audio_device;
    game_controller controllers[MAX_CONTROLLERS];

    int x_offset;
    int y_offset;
} game_state;


void game_loop(game_state* game_state);

#endif
