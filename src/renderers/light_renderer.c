#include "renderers/light_renderer.h"

#include "array.h"
#include "asset.h"
#include "log.h"
#include "allocators/allocator.h"

static void
light_renderer__init_quad_vbo(struct light_renderer *renderer)
{
    v2 quad[] = {
        { .x = -1.0f, .y = 1.0f },
        { .x = -1.0f, .y = -1.0f },
        { .x = 1.0f, .y = 1.0f },
        { .x = 1.0f, .y = -1.0f },
    };

    glGenBuffers(1, &renderer->quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->quad_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
light_renderer__init_light_vbo(struct light_renderer *renderer)
{
    glGenBuffers(1, &renderer->light_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->light_vbo);

    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
light_renderer__init_vao(struct light_renderer *renderer)
{
    static const u32 vert_size = sizeof(struct light_vertex);
    static const u32 pos_off = offsetof(struct light_vertex, pos);
    static const u32 size_off = offsetof(struct light_vertex, size);
    static const u32 color_off = offsetof(struct light_vertex, color);
    static const u32 intensity_off = offsetof(struct light_vertex, intensity);

    if (renderer->quad_vbo == 0)
    {
        log_warning("Light VBOs must be created before VAO.");
        return;
    }

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->quad_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(v2), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->light_vbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, vert_size, (void *)pos_off);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, vert_size, (void *)size_off);
    glVertexAttribPointer(3, 3, GL_FLOAT, 0, vert_size, (void *)color_off);
    glVertexAttribPointer(4, 1, GL_FLOAT, 0, vert_size, (void *)intensity_off);

    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
light_renderer_init(struct light_renderer *renderer,
                    struct engine *engine,
                    struct allocator *allocator)
{
    u32 *vert, *frag;

    array_init(renderer->lights, allocator);

    light_renderer__init_quad_vbo(renderer);
    light_renderer__init_light_vbo(renderer);
    light_renderer__init_vao(renderer);

    vert = asset_get(engine->assets, "assets/light.vert");
    frag = asset_get(engine->assets, "assets/light.frag");
    renderer->shader = shader_program_link(*vert, *frag);
}

void
light_renderer_render(struct light_renderer *renderer, u32 index)
{
    static const u32 vert_size = sizeof(struct light_vertex);

    u32 len, cap;
    len = 1;
    cap = array__cap(renderer->lights);

    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->light_vbo);

    glBufferData(GL_ARRAY_BUFFER, vert_size * cap, NULL, GL_STREAM_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    len * vert_size,
                    renderer->lights + index);

    glUseProgram(renderer->shader);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
