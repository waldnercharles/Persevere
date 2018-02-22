#ifndef RENDERER_H
#define RENDERER_H

#include "std.h"
#include "vec.h"
#include "allocators/allocator.h"

#include "renderers/sprite_renderer.h"

struct renderer_basic
{
    v2 pos;
    v3 color;
};

struct renderer_sprite
{
    v2 pos;
    v2 size;
    // r32 theta;
    v2 uv;
};

struct renderer_state
{
    u32 texture;
    u32 shader;
};

struct renderer_texture
{
    u32 id;
    s32 width;
    s32 height;
    s32 num_channels;
    const char *filename;
};

struct renderer_spritesheet
{
    struct renderer_texture *texture;
    u32 tile_width;
    u32 tile_height;
};

struct renderer_vaos
{
    u32 fbo;
    u32 geometry;
    u32 light;
    u32 shadow_caster;
    u32 shadow_geometry;
};

struct renderer_vbos
{
    u32 fbo;
    u32 quad;
    u32 sprite;
    u32 light;
    u32 shadow_caster;
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

    struct renderer_vaos vao;
    struct renderer_vbos vbo;
    struct renderer_shaders shader;
};

void renderer_init(struct renderer *renderer, struct allocator *allocator);
void renderer_render(struct renderer *renderer);

void renderer_create_texture(struct renderer *renderer,
                             const char *filename,
                             struct renderer_texture *texture);
#endif