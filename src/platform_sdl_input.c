#include "std.h"
#include "log.h"
#include "engine.h"

#include "platform_dll.h"

void
platform_open_controller(struct engine *engine, s32 joystick_index)
{
    s32 i;
    SDL_Joystick *joystick;
    SDL_GameController *controller;
    SDL_Haptic *haptic;

    (void)haptic;

    for (i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (!engine->controllers[i].connected)
        {
            break;
        }
    }

    if (i == MAX_CONTROLLERS)
    {
        return;
    }

    if (!SDL_IsGameController(joystick_index))
    {
        log_debug("Joystick at %i is not a game controller.", joystick_index);
        return;
    }

    if (!(controller = SDL_GameControllerOpen(joystick_index)))
    {
        log_warning("Failed to open controller at %i: %s",
                    joystick_index,
                    SDL_GetError());
        return;
    }

    joystick = SDL_GameControllerGetJoystick(engine->controllers[i].controller);

    haptic = SDL_HapticOpenFromJoystick(joystick);
    if (SDL_HapticRumbleInit(engine->controllers[i].haptic) != 0)
    {
        SDL_HapticClose(engine->controllers[i].haptic);
        engine->controllers[i].haptic = NULL;
    }

    engine->controllers[i].controller = controller;
    engine->controllers[i].haptic = SDL_HapticOpenFromJoystick(joystick);

    engine->controllers[i].joystick_id = SDL_JoystickInstanceID(joystick);
    engine->controllers[i].connected = true;
}

void
platform_close_controller(struct engine *engine, s32 joystick_id)
{
    s32 i;
    for (i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (engine->controllers[i].connected &&
            engine->controllers[i].joystick_id == joystick_id)
        {
            break;
        }
    }

    if (i == MAX_CONTROLLERS)
    {
        return;
    }

    if (engine->controllers[i].haptic)
    {
        SDL_HapticClose(engine->controllers[i].haptic);
    }

    if (engine->controllers[i].controller)
    {
        SDL_GameControllerClose(engine->controllers[i].controller);
    }

    engine->controllers[i].joystick_id = -1;
    engine->controllers[i].connected = false;
}

void
platform_open_all_controllers(struct engine *engine)
{
    s32 i;
    for (i = SDL_NumJoysticks() - 1; i >= 0; --i)
    {
        platform_open_controller(engine, i);
    }
}

b32
platform_handle_events(struct game_dll *dll, struct engine *engine)
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
                    case SDL_WINDOWEVENT_RESIZED:
                        glViewport(0,
                                   0,
                                   event.window.data1,
                                   event.window.data2);
                        break;

                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        engine->platform->window_size_x = event.window.data1;
                        engine->platform->window_size_y = event.window.data2;

                        dll->api->resize(engine);

                        log_debug("Resized (%i, %i)",
                                  event.window.data1,
                                  event.window.data2);
                        break;
                }
                break;

            case SDL_CONTROLLERDEVICEADDED:
                platform_open_controller(engine, event.cdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                platform_close_controller(engine, event.cdevice.which);
                break;

            case SDL_MOUSEMOTION:
                engine->platform->mouse_pos_x = event.motion.x;
                engine->platform->mouse_pos_y = event.motion.y;
                break;
        }
    }
    return true;
}
