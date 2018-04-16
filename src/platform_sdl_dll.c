#include "platform_dll.h"
#include "log.h"
#include "file.h"

#define GAME_TITLE "persevere-core"
#define DLL_FILE "./" GAME_TITLE ".dll"
#define TMP_FILE "./temp-" GAME_TITLE ".dll"
#define LOCK_FILE "./" GAME_TITLE ".lock"

void *platform_load_func(void *handle, const char *func);

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

    dll->api->start = platform_load_func(dll->handle, "game_start");
    dll->api->loop = platform_load_func(dll->handle, "game_loop");
    dll->api->init = platform_load_func(dll->handle, "engine_init");
    dll->api->unbind = platform_load_func(dll->handle, "engine_unbind");
    dll->api->bind = platform_load_func(dll->handle, "engine_bind");
    dll->api->resize = platform_load_func(dll->handle, "engine_resize");

    dll->write_time = file_write_time(TMP_FILE);
}

void *
platform_load_func(void *handle, const char *func)
{
    const char *err;
    void *func_ptr;

    func_ptr = SDL_LoadFunction(handle, func);
    if (func_ptr == NULL)
    {
        err = SDL_GetError();
        log_error("Unable to load %s: %s", func, err);
    }

    return func_ptr;
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
