#define SDL_MAIN_HANDLED
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdio.h>
#include <string.h>

#include "gl/glew.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#include "stb_image.h"

#include "dynamic_library.c"
#include "game_core.h"

#include "nerd_math.h"
#include "nerd_memory.h"
#include "nerd_mixer.h"

static struct game_state g_game_state;


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

    g_game_state.controllers[controller_index] = (const struct game_controller){0};
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


static void lock_handler(struct mixer_event *e)
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

    struct mixer_source *src = mixer_new_source_from_file("chopin.ogg");
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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_Window *window = SDL_CreateWindow("Persevere",
					  SDL_WINDOWPOS_UNDEFINED,
					  SDL_WINDOWPOS_UNDEFINED,
					  480,
					  480,
					  SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = true;
    glewInit();

    SDL_GL_SetSwapInterval(1); 

    uint shader_program = glCreateProgram();

    uint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertex_shader_src = file_cstr("vertex.glsl", NULL);

    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);
    glAttachShader(shader_program, vertex_shader);

    uint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragment_shader_src = file_cstr("fragment.glsl", NULL);

    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float vertices[] =
    {
	// position          // colors
	+0.5f, +0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top right
	+0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // bottom right
	-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, // bottom left
	-0.5f, +0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 0.0f  // top left
    };

    uint indices[] =
    {
	0, 1, 3,
	1, 2, 3
    };

    uint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* get the window surface, make a copy of it and update the window */
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    g_game_state.pixel_buffer = SDL_ConvertSurfaceFormat(surface, surface->format->format, 0);

    SDL_FreeSurface(surface);
    SDL_Log("Surface Rendered");

    struct game game;
    char *library_name = "persevere-core.dll";

    init_audio();
    open_all_game_controllers();

    load_game(library_name, &game);
    game.game_init(&g_game_state);

    uint current_time, new_time, frame_time, accumulator;
    const uint time_step = 16;

    current_time = SDL_GetTicks();
    accumulator = 0.0f;
    
    while (handle_events())
    {
	load_game_if_new(library_name, &game);
	
	if (!game.game_loop) { continue; }

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

	// TODO: lerped_physics_state = lerp(current_physics_state, previous_physics_state);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader_program);
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	game.game_loop(&g_game_state, frame_time / 1000.0f);

	SDL_GL_SwapWindow(window);
	// render_surface(window, g_game_state.pixel_buffer);
    }

    close_all_game_controllers();
    SDL_Quit();
    return 0;
}
