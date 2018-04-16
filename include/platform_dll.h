#ifndef PLATFORM_DLL_H
#define PLATFORM_DLL_H
#include "std.h"
#include "engine.h"

struct game_api
{
    void (*init)(struct platform *platform, struct engine **engine);
    void (*start)(struct engine *engine);
    void (*loop)(struct engine *engine, r32 dt);

    void (*unbind)(struct engine *engine);
    void (*bind)(struct engine *engine);

    void (*resize)(struct engine *engine);
};

struct game_dll
{
    void *handle;
    time_t write_time;

    struct game_api *api;
};
#endif