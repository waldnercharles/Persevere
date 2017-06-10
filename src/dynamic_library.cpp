#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define stat _stat
#endif

#include "SDL.h"

const std::string basePath = std::string(SDL_GetBasePath());

typedef void game_loop(SDL_Surface* surface);

struct game
{
  void* library;
  time_t lastWriteTime;
  game_loop* GameLoop;
};

static int FileExists(const std::string filename)
{
  struct stat result;
  return stat(filename.c_str(), &result) == 0;
}

static time_t GetLastWriteTime(const std::string filename)
{
  struct stat result;
  if (stat(filename.c_str(), &result) == 0)
  {
    return result.st_mtime;
  }

  return 0;
}

static void CopyFile(const std::string from, const std::string to)
{
  printf("Copying %s to %s\n", from.c_str(), to.c_str());
  std::ifstream source(from, std::ios::binary);
  std::ofstream destination(to, std::ios::binary | std::ios::trunc);
  destination << source.rdbuf();
  source.close();
  destination.close();
}

void UnloadGame(game* game)
{
  printf("Unloading Library\n");
  SDL_UnloadObject(game->library);
  game->library = 0;
  game->GameLoop = 0;
}


void LoadGame(const std::string libraryName, game* game)
{
  const std::string basePath = std::string(SDL_GetBasePath());
  const std::string lockPath = basePath + "persevere-core.lock";
  const std::string libraryPath = basePath + libraryName;
  const std::string libraryTempPath = basePath + "temp-" + libraryName;

  game->GameLoop = 0;

  if (!FileExists(lockPath))
  {
    CopyFile(libraryPath, libraryTempPath);
    
    game->library = SDL_LoadObject(libraryTempPath.c_str());
    if (!game->library)
    {
      const char* error = SDL_GetError();
      printf("Can't load library: %s\n", error);
      return;
    }
    printf("Loaded Library\n");

    game->GameLoop = (game_loop*)SDL_LoadFunction(game->library, "GameLoop");
    if (!game->GameLoop)
    {
      const char* error = SDL_GetError();
      printf("Can't load function: %s\n", error);
    }

    printf("Loaded Function!\n");
  }
  else
  {
    printf("%s exists\n", lockPath.c_str());
  }
  return;
}

void LoadGameIfNew(const std::string libraryName, game* game)
{
  const std::string libraryPath = basePath + libraryName;
  const time_t writeTime = GetLastWriteTime(libraryPath);

  if (writeTime > game->lastWriteTime)
  {
    UnloadGame(game);
    LoadGame(libraryName, game);
    if (game->GameLoop)
      game->lastWriteTime = writeTime;
  }
}
