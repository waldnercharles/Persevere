#ifndef GAME_CORE_H
#define GAME_CORE_H

#include "entity.h"
#include "SDL2/SDL.h"

#define MAX_CONTROLLERS 4


struct game_controller
{
    SDL_GameController* controller;
    SDL_Haptic* haptic;
    SDL_JoystickID joystick_id;
    int is_connected;
};


struct game_state
{
    struct echo *echo;
    
    SDL_Surface *pixel_buffer;
    SDL_AudioDeviceID audio_device;
    struct game_controller controllers[MAX_CONTROLLERS];

    int x_offset;
    int y_offset;
};


void game_init(struct game_state *game_state);
void game_loop(struct game_state *game_state, float dt);

#endif
