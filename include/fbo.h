#ifndef FBO_H
#define FBO_H
#include "std.h"
#include "vec.h"
#include "renderer.h"

struct fbo
{
    u32 vao;
    u32 vbo;

    u32 handle;
    u32 depthbuffer;
    u32 texture;

    bool enabled;

    v2 size;
    v2 viewport_pos;
    v2 viewport_size;
};

void fbo_init(struct fbo *fbo, u32 width, u32 height);
void fbo_delete(struct fbo *fbo);

void fbo_enable(struct fbo *fbo);
void fbo_disable(struct fbo *fbo);

void fbo_render(struct renderer *renderer, struct fbo *fbo);

#endif