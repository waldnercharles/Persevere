#ifndef LIGHT_RENDERER_H
#define LIGHT_RENDERER_H

#include "vec.h"
#include "allocators/allocator.h"

struct engine;

struct light_vertex
{
    v2 pos;
    v2 size;
    v3 color;
    r32 intensity;
};

struct light_renderer
{
    u32 vao;
    u32 quad_vbo;
    u32 light_vbo;

    struct array *lights;  // light_vertex

    u32 shader;
};

void light_renderer_init(struct light_renderer *renderer,
                         struct engine *engine,
                         struct allocator *allocator);

void light_renderer_render(struct light_renderer *renderer, u32 index);

#endif