#ifndef NERD_H
#define NERD_H
#include "nerd_engine.h"

struct controller
{
    void *controller;
    void *haptic;
    s32 joystick_id;
    bool is_connected;
};

#define MAX_CONTROLLERS 4

struct game
{
    struct echo *echo;
    struct mixer *mixer;
    struct renderer *renderer;
    struct asset_manager *assets;

    struct controller controllers[MAX_CONTROLLERS];
};

void nerd_init(struct game *game);

#endif