#include <stdio.h>
#include <string.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "dynamic_library.c"

#include "common.h"
#include "mixer.h"
#include "game_core.h"


static game_state g_game_state;


static void render_surface(SDL_Window *window, SDL_Surface *surface)
{
    SDL_Surface *current_surface = SDL_GetWindowSurface(window);
    SDL_BlitScaled(surface, NULL, current_surface, NULL);
    SDL_UpdateWindowSurface(window);
}


static void open_game_controller(int joystick_index)
{
    int controller_index;
    for (controller_index = 0; controller_index < MAX_CONTROLLERS; ++controller_index)
	if (!g_game_state.controllers[controller_index].is_connected) { break; }

    if (controller_index >= MAX_CONTROLLERS)
	return;

    g_game_state.controllers[controller_index].controller = SDL_GameControllerOpen(joystick_index);

    SDL_Joystick *joystick = SDL_GameControllerGetJoystick(g_game_state.controllers[controller_index].controller);

    g_game_state.controllers[controller_index].haptic = SDL_HapticOpenFromJoystick(joystick);
    if (SDL_HapticRumbleInit(g_game_state.controllers[controller_index].haptic) != 0)
    {
	SDL_HapticClose(g_game_state.controllers[controller_index].haptic);
	g_game_state.controllers[controller_index].haptic = NULL;
    }

    g_game_state.controllers[controller_index].joystick_id = SDL_JoystickInstanceID(joystick);
    g_game_state.controllers[controller_index].is_connected = true;
}


static void close_game_controller(SDL_JoystickID joystick_id)
{
    int controller_index;
    for (controller_index = 0; controller_index < MAX_CONTROLLERS; ++controller_index)
	if (g_game_state.controllers[controller_index].is_connected && g_game_state.controllers[controller_index].joystick_id == joystick_id)
	    break;

    if (controller_index >= MAX_CONTROLLERS)
	return;

    if (g_game_state.controllers[controller_index].haptic)
	SDL_HapticClose(g_game_state.controllers[controller_index].haptic);

    if (g_game_state.controllers[controller_index].controller)
	SDL_GameControllerClose(g_game_state.controllers[controller_index].controller);

    g_game_state.controllers[controller_index] = (const game_controller){0};
}


static void open_all_game_controllers()
{
    for (int joystick_index = SDL_NumJoysticks() - 1; joystick_index >= 0; --joystick_index)
	open_game_controller(joystick_index);
}


static void close_all_game_controllers()
{
    for (int joystick_index = SDL_NumJoysticks() - 1; joystick_index >= 0; --joystick_index)
	close_game_controller(joystick_index);
}


static bool handle_events()
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
			SDL_Log("Window %d resized to %dx%d",
				event.window.windowID,
				event.window.data1,
				event.window.data2);
			break;

		    case SDL_WINDOWEVENT_FOCUS_GAINED:
			SDL_Log("Window %d focused", event.window.windowID);
			break;
		}
		break;

	    case SDL_CONTROLLERDEVICEADDED:
		open_game_controller(event.cdevice.which);
		break;

	    case SDL_CONTROLLERDEVICEREMOVED:
		close_game_controller(event.cdevice.which);
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


static void audio_callback(void *userdata, uint8 *stream, int32 len)
{
    (void)(userdata);
    mixer_process((int16 *)stream, len / 2);
}


static void lock_handler(mixer_event *e)
{
    switch (e->type)
    {
	case MIXER_EVENT_LOCK:
	{
	    SDL_LockAudioDevice(g_game_state.audio_device);
	} break;
	case MIXER_EVENT_UNLOCK:
	{
	    SDL_UnlockAudioDevice(g_game_state.audio_device);
	} break;
    }
}


static void init_audio()
{
    SDL_AudioSpec want, have;
    want.format = AUDIO_S16SYS;
    want.freq = 44100; 
    want.channels = 2;
    want.userdata = NULL;
    want.samples = 512;
    want.callback = audio_callback;

    g_game_state.audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    SDL_Log("audio device opened");

    mixer_init(have.freq);
    mixer_set_lock(lock_handler);
    mixer_set_master_gain(1.0);

    SDL_PauseAudioDevice(g_game_state.audio_device, 0);

    mixer_source *src = mixer_new_source_from_file("chopin.ogg");
    if (!src)
    {
	SDL_Log("Failed to create source");
    }
    SDL_Log("Source loaded successfully");

    mixer_set_loop(src, 1);
    mixer_play(src);
}


int main()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

    SDL_Window *window = SDL_CreateWindow("Persevere",
					  SDL_WINDOWPOS_UNDEFINED,
					  SDL_WINDOWPOS_UNDEFINED,
					  1280,
					  720,
					  SDL_WINDOW_RESIZABLE);

    /* Get the window surface, make a copy of it and update the window */
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    g_game_state.pixel_buffer = SDL_ConvertSurfaceFormat(surface, surface->format->format, 0);

    SDL_FreeSurface(surface);
    SDL_Log("Surface Rendered");

    game game;
    char *library_name = "persevere-core.dll";

    init_audio();
    open_all_game_controllers();

    while (handle_events())
    {
	load_game_if_new(library_name, &game);
	render_surface(window, g_game_state.pixel_buffer);
	
	if (!game.game_loop) { continue; }
	game.game_loop(&g_game_state);
    }

    close_all_game_controllers();
    SDL_Quit();
    return 0;
}
