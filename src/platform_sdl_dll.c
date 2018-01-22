#include "platform_dll.h"
#include "log.h"
#include "file.h"

#define GAME_TITLE "persevere-core"
#define DLL_FILE "./" GAME_TITLE ".dll"
#define TMP_FILE "./temp-" GAME_TITLE ".dll"
#define LOCK_FILE "./" GAME_TITLE ".lock"

void
platform_unload_dll(struct game_dll *dll)
{
    log_debug("Unloading Dll.");

    SDL_UnloadObject(dll->handle);
    dll->handle = NULL;
    memset(dll->api, 0, sizeof(struct game_api));
}

void
platform_load_dll(struct game_dll *dll)
{
    const char *err;

    file_copy(DLL_FILE, TMP_FILE);
    log_debug("Loading " TMP_FILE);
    dll->handle = SDL_LoadObject(TMP_FILE);
    if (dll->handle == NULL)
    {
        err = SDL_GetError();
        log_warning("Couldn't load " TMP_FILE ": %s", err);
        return;
    }

    dll->api->init = SDL_LoadFunction(dll->handle, "engine_init");
    dll->api->start = SDL_LoadFunction(dll->handle, "game_start");
    dll->api->loop = SDL_LoadFunction(dll->handle, "game_loop");
    dll->api->unbind = SDL_LoadFunction(dll->handle, "engine_unbind");
    dll->api->bind = SDL_LoadFunction(dll->handle, "engine_bind");

    if (dll->api->loop == NULL)
    {
        err = SDL_GetError();
        log_warning("Couldn't load game_loop: %s", err);
        return;
    }

    dll->write_time = file_write_time(TMP_FILE);
}

void
platform_load_latest_dll(struct game_dll *dll, struct engine *engine)
{
    time_t write_time = file_write_time(DLL_FILE);
    if (write_time > dll->write_time)
    {
        if (!file_exists(LOCK_FILE))
        {
            dll->api->unbind(engine);
            platform_unload_dll(dll);
            platform_load_dll(dll);
            dll->api->bind(engine);
        }
    }
}
