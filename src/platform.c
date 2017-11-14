#define SDL_MAIN_HANDLED

#include "engine.h"
#include "game.h"

#include "dynlib.c"
#include "file.c"
#include "vec.c"
#include "mixer.c"
#include "shader.c"
#include "str.c"

#include "platform_audio.c"

static void
open_game_controller(struct game *game, s32 joystick_index)
{
    s32 i;
    for (i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (!game->controllers[i].is_connected)
        {
            break;
        }
    }

    if (i >= MAX_CONTROLLERS)
    {
        return;
    }

    game->controllers[i].controller = SDL_GameControllerOpen(joystick_index);

    SDL_Joystick *joystick =
        SDL_GameControllerGetJoystick(game->controllers[i].controller);

    game->controllers[i].haptic = SDL_HapticOpenFromJoystick(joystick);
    if (SDL_HapticRumbleInit(game->controllers[i].haptic) != 0)
    {
        SDL_HapticClose(game->controllers[i].haptic);
        game->controllers[i].haptic = NULL;
    }

    game->controllers[i].joystick_id = SDL_JoystickInstanceID(joystick);
    game->controllers[i].is_connected = true;
}

static void
close_game_controller(struct game *game, SDL_JoystickID joystick_id)
{
    s32 i;
    for (i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (game->controllers[i].is_connected &&
            game->controllers[i].joystick_id == joystick_id)
        {
            break;
        }
    }

    if (i >= MAX_CONTROLLERS)
    {
        return;
    }

    if (game->controllers[i].haptic)
    {
        SDL_HapticClose(game->controllers[i].haptic);
    }

    if (game->controllers[i].controller)
    {
        SDL_GameControllerClose(game->controllers[i].controller);
    }

    memset(&(game->controllers[i]), 0, sizeof(struct controller));
}

static void
open_all_game_controllers(struct game *game)
{
    s32 i;
    for (i = SDL_NumJoysticks() - 1; i >= 0; --i)
    {
        open_game_controller(game, i);
    }
}

static void
close_all_game_controllers(struct game *game)
{
    s32 i;
    for (i = SDL_NumJoysticks() - 1; i >= 0; --i)
    {
        close_game_controller(game, i);
    }
}

static bool
handle_events(struct game *game)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                return false;

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        break;
                }
                break;

            case SDL_CONTROLLERDEVICEADDED:
                open_game_controller(game, event.cdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                close_game_controller(game, event.cdevice.which);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                break;

            case SDL_CONTROLLERAXISMOTION:
                break;
        }
    }
    return true;
}

s32
main()
{
    u32 current_time, new_time, frame_time, accumulator;
    char *library_name = "persevere-core.dll";
    u32 time_step = 16;
    struct game_dll game_dll = { 0 };

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window =
        SDL_CreateWindow("Persevere",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         480,
                         480,
                         SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    SDL_GL_CreateContext(window);

    glewExperimental = true;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        log_error("GLEW Error: %s", glewGetErrorString(err));
    }

    SDL_GL_SetSwapInterval(1);

    struct game *game = malloc(sizeof(struct game));
    game->mixer = mixer_new();

    audio_init(game);

    open_all_game_controllers(game);

    load_game(library_name, &game_dll);
    game_dll.game_init(game);

    current_time = SDL_GetTicks();
    accumulator = 0.0f;

    while (handle_events(game))
    {
        load_game_if_new(library_name, &game_dll);

        if (!game_dll.game_loop)
        {
            continue;
        }

        new_time = SDL_GetTicks();

        frame_time = new_time - current_time;
        frame_time = math_max(frame_time, 250);

        current_time = new_time;
        accumulator += frame_time;

        while (accumulator >= time_step)
        {
            // TODO: previous_physics_state = current_physics_state;
            // TODO: physics(current_physics_state, dt);
            accumulator -= time_step;
        }

        // TODO: lerped_physics_state = lerp(current_physics_state,
        // previous_physics_state);

        game_dll.game_loop(game, frame_time / 1000.0f);

        SDL_GL_SwapWindow(window);
        // render_surface(window, game->pixel_buffer);
    }

    close_all_game_controllers(game);
    SDL_Quit();
    return 0;
}
