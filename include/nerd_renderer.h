#ifndef NERD_RENDERER_H
#define NERD_RENDERER_H
#include "nerd_engine.h"
#include "nerd_math.h"

struct renderer_sprite
{
    union vec2 pos;
    union vec2 size;
    f32 theta;
    union vec2 uv;
};

struct renderer_state
{
    u32 texture;
    u32 shader;
};

struct renderer_group
{
    struct renderer_state state;
    struct renderer_sprite *sprites;
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

struct renderer
{
    struct renderer_group *groups;
    struct renderer_sprite *sprites;
    struct renderer_state *states;

    u32 vao;
    u32 vbo[2];  // 0 = Quad, 1 = Sprites
};

struct renderer *renderer_new();
void renderer_init(struct renderer *renderer);
void renderer_render(struct renderer *renderer);

void renderer_create_texture(struct renderer *renderer,
                             const char *filename,
                             struct renderer_texture *texture);
#endif