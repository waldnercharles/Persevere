#include "nerd_dynlib.h"

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
unload_game(struct game_dll *game)
{
    printf("Unloading Library\n");
    SDL_UnloadObject(game->library);
    game->library = 0;
    game->game_loop = 0;
}

void
load_game(char *library_name, struct game_dll *game)
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

    game->game_loop = 0;

    if (!file_exists(lock_path))
    {
        file_copy(library_path, library_temp_path);

        game->library = (void *)SDL_LoadObject(library_temp_path);
        if (game->library == NULL)
        {
            err = SDL_GetError();
            printf("Can't load library: %s\n", err);
            goto cleanup;
        }
        printf("Loaded Library\n");

        game->game_init = SDL_LoadFunction(game->library, "game_init");
        game->game_loop = SDL_LoadFunction(game->library, "game_loop");

        if (game->game_loop == NULL || game->game_init == NULL)
        {
            err = SDL_GetError();
            printf("Can't load function: %s\n", err);
        }

        printf("Loaded Function!\n");
    }
    else
    {
        printf("%s exists\n", lock_path);
    }

    game->last_write_time = get_last_write_time(library_temp_path);

cleanup:
    string_free(lock_path);
    string_free(library_path);
    string_free(library_temp_path);
}

void
load_game_if_new(char *lib_name, struct game_dll *game)
{
    char *base_path = SDL_GetBasePath();
    char *lib_path = string_append_cstring(string_new(base_path), lib_name);

    const time_t write_time = get_last_write_time(lib_path);

    if (write_time > game->last_write_time)
    {
        unload_game(game);
        load_game(lib_name, game);
    }

    string_free(lib_path);
}
