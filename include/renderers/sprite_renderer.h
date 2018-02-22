#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "std.h"
#include "vec.h"
#include "allocators/allocator.h"

struct sprite_vertex
{
    v2 pos;
    v2 size;
    v2 uv;
};

struct sprite_renderer
{
    u32 vao;
    u32 quad_vbo;
    u32 sprite_vbo;

    struct sprite_vertex *sprites;
    u32 *textures;
    u32 *shaders;
};

void sprite_renderer_init(struct sprite_renderer *r,
                          struct allocator *allocator);

void sprite_renderer_render(struct sprite_renderer *r);

#endif