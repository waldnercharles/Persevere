#include "game_core.h"

void render_gradient(SDL_Surface *surface, int x_offset, int y_offset)
{
    SDL_LockSurface(surface);
    uint8_t *row = (uint8_t *)surface->pixels;

    for (int y = 0; y < surface->h; ++y)
    {
	uint32_t *pixel = (uint32_t *)row;  
	for (int x = 0; x < surface->w; ++x)
	{
	    uint8_t blue = (x + x_offset);
	    uint8_t green = (y + y_offset);

	    *pixel++ = ((green << 8) | blue);
	}
	row += surface->pitch;
    }

    SDL_UnlockSurface(surface);
}


void game_loop(game_state *game_state)
{
    render_gradient(game_state->pixel_buffer, game_state->x_offset, game_state->y_offset++);
}
