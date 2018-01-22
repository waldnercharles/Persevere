#include "dynlib.h"
#include "file.h"
#include "log.h"
#include "str.h"

#define GAME_TITLE "persevere-core"
#define DLL_FILE "./" GAME_TITLE ".dll"
#define TMP_FILE "./temp-" GAME_TITLE ".dll"
#define LOCK_FILE "./" GAME_TITLE ".lock"

static time_t
get_last_write_time(char *filename)
{
    struct stat result;
    if (stat(filename, &result) == 0)
    {
        return result.st_mtime;
    }

    return 0;
}

void
unload_game(struct game_dll *dll)
{
    char *base_path = SDL_GetBasePath();
    char *lock_path = string_new(base_path);
    lock_path = string_append_cstring(lock_path, "persevere-core.lock");

    if (!file_exists(lock_path))
    {
        log_debug("Unloading Game Dll...");
        SDL_UnloadObject(dll->library);
        dll->library = 0;
        dll->game_loop = 0;
    }

    string_free(lock_path);
}

void
load_game(char *library_name, struct game_dll *dll)
{
    char *base_path = SDL_GetBasePath();
    char *lock_path = string_new(base_path);
    char *library_path = string_new(base_path);
    char *library_temp_path = string_new(base_path);
    const char *err;

    lock_path = string_append_cstring(lock_path, "persevere-core.lock");
    library_path = string_append_cstring(library_path, library_name);

    library_temp_path = string_append_cstring(library_temp_path, "temp-");
    library_temp_path = string_append_cstring(library_temp_path, library_name);

    dll->game_loop = 0;

    if (!file_exists(lock_path))
    {
        file_copy(library_path, library_temp_path);

        log_debug("Loading Game Dll...");
        dll->library = (void *)SDL_LoadObject(library_temp_path);
        if (dll->library == NULL)
        {
            err = SDL_GetError();
            log_warning("Can't load library: %s\n", err);
            goto cleanup;
        }

        dll->game_init = SDL_LoadFunction(dll->library, "game_init");
        dll->game_loop = SDL_LoadFunction(dll->library, "game_loop");

        if (dll->game_loop == NULL || dll->game_init == NULL)
        {
            err = SDL_GetError();
            log_warning("Can't load function: %s\n", err);
        }
    }

    dll->last_write_time = get_last_write_time(library_temp_path);

cleanup:
    string_free(lock_path);
    string_free(library_path);
    string_free(library_temp_path);
}

void
load_game_if_new(char *lib_name, struct game_dll *dll)
{
    char *base_path = SDL_GetBasePath();
    char *lib_path = string_append_cstring(string_new(base_path), lib_name);

    const time_t write_time = get_last_write_time(lib_path);

    if (write_time > dll->last_write_time)
    {
        unload_game(dll);
        load_game(lib_name, dll);
    }

    string_free(lib_path);
}
