#ifndef GAME_CORE_H
#define GAME_CORE_H
#include "nerd_engine.h"

struct game;

void game_init(struct game *game);
void game_loop(struct game *game, f32 dt);
#endif