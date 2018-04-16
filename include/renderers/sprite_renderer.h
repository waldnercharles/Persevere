#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "std.h"
#include "vec.h"
#include "allocators/allocator.h"

struct sprite_vertex
{
    v3 pos;
    v2 size;
    v2 uv;
};

struct sprite_renderer
{
    u32 vao;
    u32 quad_vbo;
    u32 sprite_vbo;

    struct array *sprites;   // struct sprite_vertex
    struct array *textures;  // u32
    struct array *shaders;   // u32
};

void sprite_renderer_init(struct sprite_renderer *r,
                          struct allocator *allocator);

void sprite_renderer_render(struct sprite_renderer *r);

#endif