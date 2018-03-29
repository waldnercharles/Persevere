#include "renderers/sprite_renderer.h"
#include "array.h"
#include "vec.h"
#include "log.h"

static void
sprite_renderer__init_quad_vbo(struct sprite_renderer *renderer)
{
    v2 quad[] = {
        { .x = -0.5f, .y = +0.5f },
        { .x = -0.5f, .y = -0.5f },
        { .x = +0.5f, .y = +0.5f },
        { .x = +0.5f, .y = -0.5f },
    };
    glGenBuffers(1, &renderer->quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->quad_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
sprite_renderer__init_sprite_vbo(struct sprite_renderer *renderer)
{
    glGenBuffers(1, &renderer->sprite_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->sprite_vbo);

    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
sprite_renderer__init_vao(struct sprite_renderer *r)
{
    static const u32 size = sizeof(struct sprite_vertex);
    static const u32 pos_offset = offsetof(struct sprite_vertex, pos);
    static const u32 size_offset = offsetof(struct sprite_vertex, size);
    static const u32 uv_offset = offsetof(struct sprite_vertex, uv);

    if (r->quad_vbo == 0 || r->sprite_vbo == 0)
    {
        log_warning("Sprite VBOs must be created before VAO.");
        return;
    }

    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    glBindBuffer(GL_ARRAY_BUFFER, r->quad_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(v2), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, r->sprite_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, size, (void *)pos_offset);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, size, (void *)size_offset);
    glVertexAttribPointer(3, 2, GL_FLOAT, 0, size, (void *)uv_offset);

    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void
sprite_render_length(struct sprite_renderer *renderer, u32 start, u32 len)
{
    u32 cap, texture, shader;
    static const u32 sprite_size = sizeof(struct sprite_vertex);

    cap = array__cap(renderer->sprites);

    texture = renderer->textures[start];
    shader = renderer->shaders[start];

    glBufferData(GL_ARRAY_BUFFER, sprite_size * cap, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    len * sprite_size,
                    renderer->sprites + start);

    glUseProgram(shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
}

void
sprite_renderer_init(struct sprite_renderer *r, struct allocator *allocator)
{
    array_init(r->sprites, allocator);
    array_init(r->shaders, allocator);
    array_init(r->textures, allocator);

    sprite_renderer__init_quad_vbo(r);
    sprite_renderer__init_sprite_vbo(r);
    sprite_renderer__init_vao(r);
}

void
sprite_renderer_render(struct sprite_renderer *r)
{
    u32 i;
    u32 len, tex_len, shader_len;
    u32 start, end;
    u32 shader, texture;

    len = array__len(r->sprites);
    tex_len = array__len(r->textures);
    shader_len = array__len(r->shaders);

    if (len != shader_len || len != tex_len)
    {
        log_warning("Could not render sprites. Inconsistent array lengths.");
        return;
    }

    if (len == 0)
    {
        return;
    }

    shader = r->shaders[0];
    texture = r->textures[0];

    // TODO: Refactor buffer updates out?
    glBindVertexArray(r->vao);
    glBindBuffer(GL_ARRAY_BUFFER, r->sprite_vbo);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    start = 0;
    end = 0;

    for (i = 0; i < len; ++i, ++end)
    {
        if (r->shaders[i] != shader || r->textures[i] != texture)
        {
            sprite_render_length(r, start, end);

            start = i;
            end = 0;

            shader = r->shaders[i];
            texture = r->textures[i];
        }
    }

    sprite_render_length(r, start, end);

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}