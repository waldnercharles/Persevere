#include "renderers/shadowcaster_renderer.h"
#include "array.h"
#include "asset.h"
#include "engine.h"
#include "log.h"
#include "shader.h"

static void
shadowcaster_renderer__init_quad_vbo(struct shadowcaster_renderer *renderer)
{
    v2 quad[] = {
        { .x = 0.0f, .y = 1.0f },
        { .x = 0.0f, .y = 0.0f },
        { .x = 1.0f, .y = 1.0f },
        { .x = 1.0f, .y = 0.0f },
    };

    glGenBuffers(1, &renderer->quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->quad_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
shadowcaster_renderer__init_caster_vbo(struct shadowcaster_renderer *renderer)
{
    glGenBuffers(1, &renderer->caster_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->caster_vbo);

    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
shadowcaster_renderer__init_vao(struct shadowcaster_renderer *renderer)
{
    static const u32 size = sizeof(struct shadowcaster_vertex);
    static const u32 pos_offset = offsetof(struct shadowcaster_vertex, pos);
    static const u32 size_offset = offsetof(struct shadowcaster_vertex, size);

    if (renderer->quad_vbo == 0)
    {
        log_warning("Shadow Caster VBOs must be created before VAO.");
        return;
    }

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->quad_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(v2), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->caster_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, size, (void *)pos_offset);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, size, (void *)size_offset);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
shadowcaster_renderer_init(struct engine *engine,
                           struct shadowcaster_renderer *renderer,
                           struct allocator *allocator)
{
    u32 *vert, *frag;

    array_init(renderer->casters, allocator);

    shadowcaster_renderer__init_quad_vbo(renderer);
    shadowcaster_renderer__init_caster_vbo(renderer);
    shadowcaster_renderer__init_vao(renderer);

    vert = asset_get(engine->assets, "assets/shadowcaster.vert");
    frag = asset_get(engine->assets, "assets/shadowcaster.frag");
    renderer->shader = shader_program_link(*vert, *frag);
}

void
shadowcaster_renderer_render(struct shadowcaster_renderer *renderer)
{
    static const u32 vert_size = sizeof(struct shadowcaster_vertex);

    u32 len, cap;

    len = array__len(renderer->casters);
    cap = array__cap(renderer->casters);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->caster_vbo);

    glBufferData(GL_ARRAY_BUFFER, vert_size * cap, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, len * vert_size, renderer->casters);

    glUseProgram(renderer->shader);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}