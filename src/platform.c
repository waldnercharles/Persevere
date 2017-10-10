#define SDL_MAIN_HANDLED

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "game_core.h"
#include "nerd.h"
#include "nerd_memory.h"

#include "nerd_dynlib.c"
#include "nerd_file.c"
#include "nerd_math.c"
#include "nerd_mixer.c"
#include "nerd_shader.c"
#include "nerd_string.c"

static struct game game;

static void
open_game_controller(int joystick_index)
{
    int i;
    for (i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (!game.controllers[i].is_connected)
        {
            break;
        }
    }

    if (i >= MAX_CONTROLLERS)
    {
        return;
    }

    game.controllers[i].controller = SDL_GameControllerOpen(joystick_index);

    SDL_Joystick *joystick =
        SDL_GameControllerGetJoystick(game.controllers[i].controller);

    game.controllers[i].haptic = SDL_HapticOpenFromJoystick(joystick);
    if (SDL_HapticRumbleInit(game.controllers[i].haptic) != 0)
    {
        SDL_HapticClose(game.controllers[i].haptic);
        game.controllers[i].haptic = NULL;
    }

    game.controllers[i].joystick_id = SDL_JoystickInstanceID(joystick);
    game.controllers[i].is_connected = true;
}

static void
close_game_controller(SDL_JoystickID joystick_id)
{
    int i;
    for (i = 0; i < MAX_CONTROLLERS; ++i)
    {
        if (game.controllers[i].is_connected &&
            game.controllers[i].joystick_id == joystick_id)
        {
            break;
        }
    }

    if (i >= MAX_CONTROLLERS)
    {
        return;
    }

    if (game.controllers[i].haptic)
    {
        SDL_HapticClose(game.controllers[i].haptic);
    }

    if (game.controllers[i].controller)
    {
        SDL_GameControllerClose(game.controllers[i].controller);
    }

    game.controllers[i] = (const struct game_controller){ 0 };
}

static void
open_all_game_controllers()
{
    int i;
    for (i = SDL_NumJoysticks() - 1; i >= 0; --i)
    {
        open_game_controller(i);
    }
}

static void
close_all_game_controllers()
{
    int i;
    for (i = SDL_NumJoysticks() - 1; i >= 0; --i)
    {
        close_game_controller(i);
    }
}

static bool
handle_events()
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

static void
audio_callback(void *userdata, uint8 *stream, int32 len)
{
    struct mixer *mixer;
    if (userdata == NULL)
    {
        return;
    }

    mixer = (struct mixer *)userdata;
    mixer_process(mixer, (int16 *)stream, len / 2);
}

static void
lock_handler(struct mixer_event *e)
{
    switch (e->type)
    {
        case MIXER_EVENT_LOCK:
            SDL_LockAudioDevice(game.audio_device);
            break;
        case MIXER_EVENT_UNLOCK:
            SDL_UnlockAudioDevice(game.audio_device);
            break;
    }
}

static void
init_audio()
{
    struct mixer *mixer = malloc(sizeof(struct mixer));
    SDL_AudioSpec want, have;
    want.format = AUDIO_S16SYS;
    want.freq = 44100;
    want.channels = 2;
    want.userdata = NULL;
    want.samples = 512;
    want.userdata = mixer;
    want.callback = audio_callback;

    game.audio_device = SDL_OpenAudioDevice(NULL,
                                            0,
                                            &want,
                                            &have,
                                            SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    SDL_Log("audio device opened");

    game.mixer = mixer;
    mixer_init(game.mixer, have.freq);
    mixer_set_lock(game.mixer, lock_handler);
    mixer_set_master_gain(game.mixer, 1.0);

    SDL_PauseAudioDevice(game.audio_device, 0);

    struct mixer_source *src =
        mixer_new_source_from_file(game.mixer, "chopin.ogg");
    if (!src)
    {
        SDL_Log("Failed to create source");
    }
    SDL_Log("Source loaded successfully");
    mixer_set_loop(src, 1);
    mixer_play(game.mixer, src);
}

int
main()
{
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
        fprintf(stderr, "error: %s\n", glewGetErrorString(err));
    }

    glEnable(GL_DEPTH_TEST);

    SDL_GL_SetSwapInterval(1);

    game.shader = shader_program_load("vertex.glsl", "fragment.glsl");

    // clang-format off
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // clang-format on

    uint VBO;
    glGenVertexArrays(1, &game.VAO);

    glGenBuffers(1, &VBO);

    glBindVertexArray(game.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void *)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    uint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    void *data = stbi_load("crate.jpg", &width, &height, &channels, 0);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width,
                 height,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    struct game_dll game_dll = { 0 };
    char *library_name = "persevere-core.dll";

    init_audio();
    open_all_game_controllers();

    load_game(library_name, &game_dll);
    game_dll.game_init(&game);

    uint current_time, new_time, frame_time, accumulator;
    const uint time_step = 16;

    current_time = SDL_GetTicks();
    accumulator = 0.0f;

    while (handle_events())
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
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        game_dll.game_loop(&game, frame_time / 1000.0f);

        SDL_GL_SwapWindow(window);
        // render_surface(window, game.pixel_buffer);
    }

    close_all_game_controllers();
    SDL_Quit();
    return 0;
}
