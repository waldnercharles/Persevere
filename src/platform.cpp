#include <fstream>
#include <iostream>
#include <experimental/filesystem>
#include <filesystem>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "game_core.h"

namespace fs = std::experimental::filesystem::v1;

const char* libraryPath = strcat(SDL_GetBasePath(), "persevere-core.dll");
const char* libraryTempPath = strcat(SDL_GetBasePath(), "persevere-core-temp.dll");

const char* lockPath = strcat(SDL_GetBasePath(), "lock.tmp");

typedef void(*GameLoopFunc)();

void* library;
GameLoopFunc gameLoop;

int count = 0;
void UnloadCourage()
{
	SDL_UnloadObject(library);
	library = 0;
	gameLoop = 0;
}

void LoadCourage()
{
	if (!fs::exists(lockPath)) {
		count++;
		printf("load: %d\n", count);
		//CopyFile(libraryPath, libraryTempPath, 0);

		std::ifstream source(libraryPath, std::ios::binary);
		std::ofstream destination(libraryTempPath, std::ios::binary | std::ios::trunc);
		destination << source.rdbuf();
		source.close();
		destination.close();

		library = SDL_LoadObject(libraryTempPath);
		if (!library) {
			auto error = SDL_GetError();
			printf("Can't load library: %s\n", error);
		}
		else {
			gameLoop = (GameLoopFunc)SDL_LoadFunction(library, "GameLoop");
			if (!gameLoop) {
				auto error = SDL_GetError();
				printf("Can't load function: %s\n", error);
			}
		}
	}
}

int main(int, char**)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	LoadCourage();
	fs::file_time_type lastWriteTime = fs::last_write_time(libraryPath);
	while (1) {
		const auto writeTime = fs::last_write_time(libraryPath);
		if (!fs::exists(strcat(SDL_GetBasePath(), "persevere-core.lock")) && writeTime != lastWriteTime) {
			lastWriteTime = writeTime;
			UnloadCourage();
			LoadCourage();
		}
		gameLoop();
	}
	SDL_Quit();
	return 0;
}