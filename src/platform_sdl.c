#define SDL_MAIN_HANDLED
#include "std.h"
#include "log.h"

#include "platform.h"
#include "engine.h"

#include "allocators/allocator.c"
#include "allocators/linear_allocator.c"
#include "file.c"

#include "platform_audio.c"
#include "platform_sdl_audio.c"
#include "platform_sdl_dll.c"
#include "platform_sdl_input.c"

void
platform_glew_init()
{
    GLenum err;
    glewExperimental = true;
    err = glewInit();
    if (err != GLEW_OK)
    {
        log_error("GLEW Error: %s", glewGetErrorString(err));
    }
}

struct memory *
platform_memory_init()
{
    struct memory *memory;
    struct linear_allocator *linear_allocator;

    memory = malloc(sizeof(struct memory));

    linear_allocator = malloc(sizeof(struct linear_allocator));
    linear_allocator_init(linear_allocator, megabytes(64));

    memory->permanent = malloc(sizeof(struct allocator));
    allocator_init(memory->permanent,
                   linear_allocator,
                   linear_allocator_alloc,
                   linear_allocator_dealloc);

    return memory;
}

struct platform_api *
platform_api_init()
{
    struct platform_api *api;

    api = malloc(sizeof(struct platform_api));

    api->alloc = alloc;

    api->open_audio_device = platform_open_audio_device;
    api->close_audio_device = platform_close_audio_device;
    api->pause_audio_device = platform_pause_audio_device;
    api->lock_audio_device = platform_lock_audio_device;

    return api;
}

s32
main()
{
    struct platform *platform;
    struct engine *engine;

    struct game_dll *dll;

    u32 prev_time, curr_time, frame_time;
    u32 time_step, accumulator;

    dll = malloc(sizeof(struct game_dll));
    dll->api = malloc(sizeof(struct game_api));

    platform = malloc(sizeof(struct platform));
    platform->memory = platform_memory_init();
    platform->api = platform_api_init();

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    platform->window =
        SDL_CreateWindow("Persevere",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         512,
                         512,
                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    platform->gl = SDL_GL_CreateContext(platform->window);

    platform_glew_init();
    if (SDL_GL_SetSwapInterval(1) > 0)
    {
        log_warning("Unable to set vsync: %s", SDL_GetError());
    }

    platform_load_dll(dll);

    dll->api->init(platform, &engine);
    dll->api->start(engine);

    prev_time = SDL_GetTicks();

    time_step = 1000.0f / 60.0f;
    accumulator = 0.0f;

    while (platform_handle_events(engine))
    {
        platform_load_latest_dll(dll, engine);

        if (dll->api->loop == NULL)
        {
            continue;
        }

        curr_time = SDL_GetTicks();

        frame_time = curr_time - prev_time;
        frame_time = math_max(frame_time, 250);

        prev_time = curr_time;
        accumulator += frame_time;

        while (accumulator >= time_step)
        {
            // TODO: prev_physics = curr_physics
            // TODO: curr_physics = physics(curr_physics, dt);
            accumulator -= time_step;
        }

        // TODO: lerp_physics = physics_lerp(curr_physics, prev_physics);

        dll->api->loop(engine, frame_time / 1000.0f);
        SDL_GL_SwapWindow(platform->window);
    }

    SDL_Quit();
    return 0;
}