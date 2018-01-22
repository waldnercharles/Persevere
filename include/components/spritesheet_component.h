#ifndef SPRITESHEET_COMPONENT_H
#define SPRITESHEET_COMPONENT_H

#include "std.h"

struct texture
{
    u32 id;
    u32 width;
    u32 height;
    u32 num_channels;
    char *filename;
};

struct spritesheet
{
    u32 tile_width;
    u32 tile_height;
};

#endif