#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <windows.h> /* gross! */

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define stat _stat
#endif

#include "SDL2/SDL.h"

#include "nerd_file.h"
#include "nerd_string.h"


typedef void game_init_func(void *game_state);
typedef void game_loop_func(void *game_state);

struct game
{
    void *library;
    time_t last_write_time;
    game_init_func *game_init;
    game_loop_func *game_loop;
};


static time_t get_last_write_time(char *filename)
{
    struct stat result;
    if (stat(filename, &result) == 0)
    {
	return result.st_mtime;
    }

    return 0;
}


void unload_game(struct game *game)
{
    printf("Unloading Library\n");
    SDL_UnloadObject(game->library);
    game->library = 0;
    game->game_loop = 0;
}


void load_game(char *library_name, struct game *game)
{
    char *base_path = SDL_GetBasePath();
    char *lock_path = string_new(base_path);
    char *library_path = string_new(base_path);
    char *library_temp_path = string_new(base_path);

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
	    const char *error = SDL_GetError();
	    printf("Can't load library: %s\n", error);
	    goto cleanup;
	}
	printf("Loaded Library\n");

	game->game_init = (game_init_func *)SDL_LoadFunction(game->library, "game_init");
	game->game_loop = (game_loop_func *)SDL_LoadFunction(game->library, "game_loop");
	if (game->game_loop == NULL || game->game_init == NULL)
	{
	    const char *error = SDL_GetError();
	    printf("Can't load function: %s\n", error);
	}

	printf("Loaded Function!\n");
    }
    else
    {
	printf("%s exists\n", lock_path);
    }
    
    cleanup:
    string_free(lock_path);
    string_free(library_path);
    string_free(library_temp_path);
    return;
}


void load_game_if_new(char *library_name, struct game *game)
{
    char *base_path = SDL_GetBasePath();
    char *library_path = string_new(base_path);
    library_path = string_append_cstring(library_path, library_name);

    const time_t write_time = get_last_write_time(library_path);

    if (write_time > game->last_write_time)
    {
	unload_game(game);
	load_game(library_name, game);
	if (game->game_loop)
	    game->last_write_time = write_time;
    }

    string_free(library_path);
}
