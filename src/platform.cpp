#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "dynamic_library.cpp"

#include "common.h"
#include "mixer.h"
#include "game_core.h"

static SDL_Surface* gPixelBuffer = nullptr;
//static uint32_t gPixelBufferWidth;
//static uint32_t gPixelBufferHeight;
//static uint32_t gBytesPerPixel = 4;

void RenderSurface(SDL_Window* window, SDL_Surface* surface)
{
  SDL_Surface* currentSurface = SDL_GetWindowSurface(window);
  SDL_BlitScaled(surface, nullptr, currentSurface, nullptr);
  SDL_UpdateWindowSurface(window);
}

static bool HandleEvents()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_QUIT:
	return false;

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
      } break;
    }
  }
  return true;
}

static SDL_AudioDeviceID gAudioDevice;

static void AudioCallback(void* userdata, uint8* stream, int32 len)
{
  (void)(userdata);
  MixerProcess((int16*)stream, len / 2);
}

static void LockHandler(MixerEvent* e)
{
  switch (e->type)
  {
    case MIXER_EVENT_LOCK:
    {
      SDL_LockAudioDevice(gAudioDevice);
    } break;
    case MIXER_EVENT_UNLOCK:
    {
      SDL_UnlockAudioDevice(gAudioDevice);
    } break;
  }
}

void InitAudio()
{
  SDL_AudioSpec want, have;
  want.format = AUDIO_S16SYS;
  want.freq = 44100; 
  want.channels = 2;
  want.userdata = NULL;
  want.samples = 512;
  want.callback = AudioCallback;

  gAudioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

  SDL_Log("audio device opened");

  MixerInit(have.freq);
  MixerSetLock(LockHandler);
  MixerSetMasterGain(1.0);

  SDL_PauseAudioDevice(gAudioDevice, 0);

  //cm_Source* src = cm_new_source_from_mem((void*)wav->memoryStream.data, wav->memoryStream.dataSize);
  MixerSource* src = MixerNewSourceFromFile("chopin.ogg");
  //SDL_Log("cmixer: %i", src->length);
  if (!src)
  {
    SDL_Log("Failed to create source");
  }
  SDL_Log("Source loaded successfully");

  MixerSetLoop(src, 1);
  MixerPlay(src);
}

int main()
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);


  SDL_Window* window = SDL_CreateWindow("Persevere",
					SDL_WINDOWPOS_UNDEFINED,
					SDL_WINDOWPOS_UNDEFINED,
					1280,
					720,
					SDL_WINDOW_RESIZABLE);

// Get the window surface, make a copy of it and update the window
  SDL_Surface *surface = SDL_GetWindowSurface(window);
  gPixelBuffer = SDL_ConvertSurfaceFormat(surface, surface->format->format, 0);
  RenderSurface(window, gPixelBuffer);
  SDL_FreeSurface(surface);
  SDL_Log("Surface Rendered");

  game game = {};
  const std::string libraryName = "persevere-core.dll";
  
  InitAudio();
  printf("Audio started");
  while (HandleEvents())
  {
    LoadGameIfNew(libraryName, &game);
    RenderSurface(window, gPixelBuffer);

    if (!game.GameLoop) { continue; }
    game.GameLoop(gPixelBuffer);
  }

  SDL_Quit();
  return 0;
}
