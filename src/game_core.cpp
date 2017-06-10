#include <iostream>
#include "game_core.h"

void RenderGradient(SDL_Surface* surface, int xOffset, int yOffset)
{
  SDL_LockSurface(surface);
  uint8_t* row = (uint8_t*)surface->pixels;

  for (int y = 0; y < surface->h; ++y)
  {
    uint32_t* pixel = (uint32_t*)row;
    for (int x = 0; x < surface->w; ++x)
    {
      uint8_t blue = (x + xOffset);
      uint8_t green = (y + yOffset);

      *pixel++ = ((green << 16) | blue);
    }
    row += surface->pitch;
  }

  SDL_UnlockSurface(surface);
}


EXPORT void GameLoop(SDL_Surface* surface)
{
  static int xOffset = 0;
  static int yOffset = 0;
  RenderGradient(surface, xOffset++, yOffset);
}
