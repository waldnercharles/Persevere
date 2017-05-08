#include <iostream>
#include "game_core.h"

int i = 0;

EXPORT void GameLoop()
{
	if (i++ > 5000) {
		printf("Hello Banana!\n");
		i = 0;
	}
}
