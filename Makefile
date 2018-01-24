CC          = gcc
CFLAGS      = -O0 -g -ggdb -Werror -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -std=c99 -Wno-missing-braces -Wno-implicit-fallthrough -Wno-maybe-uninitialized

ROOT        = c:/msys64/mingw32
PRJ         = persevere
BIN         = bin
SRC         = src
INCLUDE     = include
ASSETS      = assets
SHADERS     = shaders

SYS_INCLUDE = -isystem $(ROOT)/include -isystem third_party -I $(INCLUDE)

all: game platform assets

# pre: $(INCLUDE)/nerd_engine.h
# 	@echo "Precompiling engine header..."
# 	@$(CC) $(CFLAGS) $(SYS_INCLUDE) $(INCLUDE)/nerd_engine.h
# 	@echo "Precompiling engine header...Done"

game: $(SRC)/game.c
	@echo "Building Game..."
	@echo "WAITING FOR COMPILE..." > $(BIN)/$(PRJ)-core.lock
	@$(CC) $(CFLAGS) $(SYS_INCLUDE) -shared $(SRC)/game.c -o $(BIN)/$(PRJ)-core.dll -lopengl32 -lglew32
	@echo "Building Game...Done"
	@$(RM) $(BIN)/$(PRJ)-core.lock

platform: $(SRC)/platform_sdl.c
	@echo "Building Platform..."
	@$(CC) $(CFLAGS) $(SYS_INCLUDE) $(SRC)/platform_sdl.c -o $(BIN)/$(PRJ).exe -lSDL2 -lSDL2main -lopengl32 -lglew32
	@echo "Building Platform...Done."

assets:
	@echo "Copying Assets..."
	@cp -u $(ROOT)/bin/SDL2.dll $(BIN)
	@cp -u $(ASSETS)/* $(BIN)/$(ASSETS)
	@echo "Copying Assets...Done"

clean:
	@$(RM) $(BIN)/$(PRJ).exe
	@$(RM) $(BIN)/$(PRJ)-core.dll
	@$(RM) $(BIN)/SDL2.dll

.PHONY: all assets clean
