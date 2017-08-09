CC          = gcc
CP          = cp -n
CFLAGS      = -g -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -std=c99

ROOT        = c:/msys64/mingw32
PRJ         = persevere
BIN         = bin
SRC         = src
INCLUDE     = include
RESOURCES   = resources

SYS_INCLUDE = -isystem $(ROOT)/include -isystem $(INCLUDE)


all: game platform resources

game: $(SRC)/game_core.c
	@echo "WAITING FOR COMPILE..." > $(PRJ)-core.lock
	@$(CC) $(CFLAGS) $(SYS_INCLUDE) -shared $(SRC)/game_core.c -o $(BIN)/$(PRJ)-core.dll -lSDL2
	@$(RM) $(PRJ)-core.lock

platform: $(SRC)/platform.c
	@$(CC) $(CFLAGS) $(SYS_INCLUDE) $(SRC)/platform.c -o $(BIN)/$(PRJ).exe -lSDL2 -lSDL2main

resources:
	@$(CP) $(ROOT)/bin/SDL2.dll $(BIN)
	@$(CP) $(RESOURCES)/* $(BIN)

clean:
	@$(RM) $(BIN)/$(PRJ).exe
	@$(RM) $(BIN)/$(PRJ)-core.dll
	@$(RM) $(BIN)/SDL2.dll

.PHONY: all resources clean
