#include <iostream>
#include "courage.h"

int i = 0;

EXPORT void GameLoop()
{
	if (i++ > 5000) {
		printf("Hello Sweety!\n");
		i = 0;
	}
}
