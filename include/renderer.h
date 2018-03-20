#ifndef RENDERER_H
#define RENDERER_H

#include "std.h"
#include "vec.h"

#include "allocators/allocator.h"

#include "renderers/sprite_renderer.h"
#include "renderers/light_renderer.h"
#include "renderers/shadowcaster_renderer.h"

struct engine;

struct renderer_basic
{
    v2 pos;
    v3 color;
};

struct renderer_texture
{
    u32 id;
    s32 width;
    s32 height;
    s32 num_channels;
    const char *filename;
};

struct renderer_vaos
{
    u32 fbo;
    u32 geometry;
    u32 light;
    u32 shadowcaster;
    u32 shadow_geometry;
};

struct renderer_vbos
{
    u32 fbo;
    u32 quad;
    u32 sprite;
    u32 light;
    u32 shadowcaster;
    u32 shadow_geometry;
};

struct renderer_shaders
{
    u32 sprite;  // For instanced quad rendering with a texture
    u32 basic;   // For simple rendering
    u32 light;   // For rendering lights
    u32 fbo;     // For rendering the main screen fbo
};

struct renderer
{
    struct fbo *fbo;

    struct sprite_renderer *sprite_renderer;
    struct light_renderer *light_renderer;
    struct shadowcaster_renderer *shadowcaster_renderer;

    struct renderer_vaos vao;
    struct renderer_vbos vbo;
    struct renderer_shaders shader;
};

void renderer_init(struct renderer *renderer,
                   struct engine *engine,
                   struct allocator *allocator);

void renderer_render(struct renderer *renderer);

void renderer_create_texture(struct renderer *renderer,
                             const char *filename,
                             struct renderer_texture *texture);
#endif