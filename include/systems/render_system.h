#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H
#include "nerd_engine.h"

struct echo;

u32 render_system;
void render_system_process_begin(struct echo *echo, void *u_data);
void render_system_process(struct echo *echo, void *u_data, u32 entity, f32 dt);
void render_system_process_end(struct echo *echo, void *u_data);
#endif