#pragma once
#include "SDL.h"
#define EXPORT extern "C" __declspec(dllexport)

EXPORT void GameLoop(SDL_Surface* surface);

