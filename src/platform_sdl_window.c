#include "platform_window.h"

void
platform_get_window_size(void *window, s32 *x, s32 *y)
{
    SDL_GetWindowSize(window, x, y);
}

void
platform_set_window_size(void *window, s32 x, s32 y)
{
    SDL_SetWindowSize(window, x, y);
}