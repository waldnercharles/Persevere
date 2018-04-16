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
    b32 connected;
};

struct engine
{
    struct platform *platform;

    struct ecs *ecs;
    struct mixer *mixer;
    struct renderer *renderer;
    struct asset_manager *assets;

    struct controller controllers[MAX_CONTROLLERS];

    b32 running;
};

void engine_open_audio_device(struct platform_api *api, struct mixer *mixer);
void engine_close_audio_device(struct platform_api *api, struct mixer *mixer);

export void engine_init(struct platform *platform, struct engine **engine);

export void engine_bind(struct engine *engine);
export void engine_unbind(struct engine *engine);

export void engine_resize(struct engine *engine);
#endif