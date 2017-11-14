#ifndef DYNLIB_H
#define DYNLIB_H
#include "std.h"

typedef void game_init_func(void *game_state);
typedef void game_loop_func(void *game_state, f32 dt);

struct game_dll
{
    void *library;
    time_t last_write_time;
    game_init_func *game_init;
    game_loop_func *game_loop;
};

void unload_game(struct game_dll *game);
void load_game(char *library_name, struct game_dll *game);
void load_game_if_new(char *library_name, struct game_dll *game);
#endif