#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <stdio.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif

#if defined(_WIN32)
#define stat _stat
#endif

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "game_core.h"

const std::string libraryPath = std::string(SDL_GetBasePath()) + "persevere-core.dll";
const std::string libraryTempPath = std::string(SDL_GetBasePath()) + "persevere-core-temp.dll";

const std::string lockPath = std::string(SDL_GetBasePath()) + "lock.tmp";

typedef void(*GameLoopFunc)();

void* library;
GameLoopFunc gameLoop;

int FileExists(const char* filename)
{
  struct stat result;
  return stat(filename, &result) == 0;
}

time_t GetLastWriteTime(const char* filename)
{
  struct stat result;
  if (stat(filename, &result) == 0) {
    return result.st_mtime;
  }

  return -1;
}

void CopyFile(const char* from, const char* to)
{
  std::ifstream source(from, std::ios::binary);
  std::ofstream destination(to, std::ios::binary | std::ios::trunc);
  destination << source.rdbuf();
  source.close();
  destination.close();
}

void UnloadGame()
{
  SDL_UnloadObject(library);
  library = 0;
  gameLoop = 0;
}

void LoadGame()
{
  if (!FileExists(lockPath.c_str())) {
    CopyFile(libraryPath.c_str(), libraryTempPath.c_str());
    
    library = SDL_LoadObject("persevere-core-temp.dll");
    if (!library) {
      const char* error = SDL_GetError();
      printf("Can't load library: %s\n", error);
    }
    else {
      gameLoop = (GameLoopFunc)SDL_LoadFunction(library, "GameLoop");
      if (!gameLoop) {
	const char* error = SDL_GetError();
	printf("Can't load function: %s\n", error);
      }
    }
  }
}

int main()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  LoadGame();
  time_t lastWriteTime = GetLastWriteTime(libraryPath.c_str());
  while (1) {
    const time_t writeTime = GetLastWriteTime(libraryPath.c_str());
    const char* buildLockfile = (std::string(SDL_GetBasePath()) + "persevere-core.lock").c_str();
    if (!FileExists(buildLockfile)) {
      if (writeTime != lastWriteTime) {
	lastWriteTime = writeTime;
	UnloadGame();
	LoadGame();
      }
      if (gameLoop) {
	gameLoop();
      }
    }
  }
  SDL_Quit();
  return 0;
}
