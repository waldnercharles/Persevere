#include <fstream>
#include <iostream>
#include <stdio.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "dynamic_library.cpp"
#include "game_core.h"

SDL_Surface* backbuffer = nullptr;
bool isRunning = true;

void RenderSurface(SDL_Window* window, SDL_Surface* surface)
{
  SDL_Surface* currentSurface = SDL_GetWindowSurface(window);
  SDL_BlitScaled(surface, nullptr, currentSurface, nullptr);
  SDL_UpdateWindowSurface(window);
}

void HandleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_QUIT:
      {
	isRunning = false;
      } break;

      case SDL_WINDOWEVENT:
      {
	switch (event.window.event)
	{
	  case SDL_WINDOWEVENT_SIZE_CHANGED:
	  {
	    SDL_Log("Window %d resized to %dx%d",
		    event.window.windowID,
		    event.window.data1,
		    event.window.data2);
	  } break;

	  case SDL_WINDOWEVENT_FOCUS_GAINED:
	  {
	    SDL_Log("Window %d focused",
		    event.window.windowID);
	  } break;
	}
      }
    }
  }
}

int main()
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

  // TODO: Init Audio

  SDL_Window* window = SDL_CreateWindow("Persevere",
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					640,
					480,
					SDL_WINDOW_RESIZABLE);

  // Get window's surface and copy to a backbuffer
  SDL_Surface* surface = SDL_GetWindowSurface(window);
  backbuffer = SDL_ConvertSurfaceFormat(surface, surface->format->format, 0);
  RenderSurface(window, backbuffer);
  SDL_FreeSurface(surface);

  game game = {};
  const std::string libraryName = "persevere-core.dll";

  while (isRunning)
  {
    LoadGameIfNew(libraryName, &game);
    if (!game.GameLoop) { continue; }

    HandleEvents();
    game.GameLoop();
  }

  SDL_Quit();
  return 0;
}
