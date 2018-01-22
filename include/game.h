#ifndef GAME_CORE_H
#define GAME_CORE_H
#include "std.h"
#include "engine.h"

void game_start(struct engine *engine);
void game_loop(struct engine *engine, r32 dt);

#endif