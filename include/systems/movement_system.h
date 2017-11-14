#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include "nerd_echo.h"

u32 movement_system;

void movement_system_process(struct echo *echo,
                             void *u_data,
                             u32 entity,
                             f32 dt);
#endif