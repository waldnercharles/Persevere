#ifndef ENGINE_H
#define ENGINE_H
#include "std.h"

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
    struct ecs *ecs;
    struct mixer *mixer;
    struct renderer *renderer;
    struct asset_manager *assets;

    struct controller controllers[MAX_CONTROLLERS];
};

void engine_init(struct game *game);

#endif