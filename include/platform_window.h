#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H
#include "std.h"

void platform_get_window_size(void *window, s32 *x, s32 *y);
void platform_set_window_size(void *window, s32 x, s32 y);

#endif