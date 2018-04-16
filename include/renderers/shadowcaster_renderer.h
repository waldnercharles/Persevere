#ifndef SHADOWCASTER_RENDERER_H
#define SHADOWCASTER_RENDERER_H

#include "std.h"
#include "vec.h"

#include "allocators/allocator.h"

struct engine;

struct shadowcaster_vertex
{
    v2 pos;
    v2 size;
};

struct shadowcaster_renderer
{
    u32 vao;
    u32 quad_vbo;
    u32 caster_vbo;

    struct array *casters;  // struct shadowcaster_vertex

    u32 shader;
};

void shadowcaster_renderer_init(struct engine *engine,
                                struct shadowcaster_renderer *renderer,
                                struct allocator *allocator);

void shadowcaster_renderer_render(struct shadowcaster_renderer *renderer);

#endif