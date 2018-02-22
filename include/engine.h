#ifndef ENGINE_H
#define ENGINE_H
#include "std.h"

#include "memory.h"

#include "ecs.h"
#include "mixer.h"
#include "renderer.h"
#include "asset.h"

#include "platform.h"

#define MAX_CONTROLLERS 4

struct controller
{
    void *controller;
    void *haptic;
    s32 joystick_id;
    bool connected;
};

struct engine
{
    struct platform *platform;

    struct ecs *ecs;
    struct mixer *mixer;
    struct renderer *renderer;
    struct asset_manager *assets;

    struct controller controllers[MAX_CONTROLLERS];

    bool running;
};

void engine_init(struct platform *platform, struct engine **engine);
void engine_refresh_bindings(struct engine *engine);

#endif