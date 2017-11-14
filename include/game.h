#ifndef GAME_CORE_H
#define GAME_CORE_H
#include "std.h"

struct game;

void game_init(struct game *game);
void game_loop(struct game *game, f32 dt);
#endif