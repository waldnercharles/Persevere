#include "renderer.h"
#include "array.h"
#include "shader.h"

#include "stb_image.h"

void
renderer_init(struct renderer *renderer, struct allocator *allocator)
{
    r32 quad[] = {
        0.0f, 1.0f,  // bottom left
        0.0f, 0.0f,  // top left
        1.0f, 1.0f,  // bottom right
        1.0f, 0.0f,  // top right
    };

    array_init(renderer->sprites, allocator);
    array_init(renderer->states, allocator);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // Accept fragments closer to the camera
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    glGenVertexArrays(1, &(renderer->vao));
    glBindVertexArray(renderer->vao);

    // TODO: Get Uniforms

    // Quad Buffer
    glGenBuffers(1, &(renderer->vbo[0]));
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 2 * sizeof(r32), (void *)0);
    glEnableVertexAttribArray(0);

    // Sprite Buffer
    glGenBuffers(1, &(renderer->vbo[1]));
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STREAM_DRAW);

    // renderer->shader = shader_program_load("vertex.glsl", "fragment.glsl");
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[1]);

    const u32 sprite_size = sizeof(struct renderer_sprite);
    const u32 pos_offset = offsetof(struct renderer_sprite, pos);
    const u32 size_offset = offsetof(struct renderer_sprite, size);
    const u32 uv_offset = offsetof(struct renderer_sprite, uv);
    const u32 theta_offset = offsetof(struct renderer_sprite, theta);

    glVertexAttribPointer(1, 2, GL_FLOAT, 0, sprite_size, (void *)pos_offset);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, sprite_size, (void *)size_offset);
    glVertexAttribPointer(3, 2, GL_FLOAT, 0, sprite_size, (void *)uv_offset);
    glVertexAttribPointer(4, 1, GL_FLOAT, 0, sprite_size, (void *)theta_offset);

    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    // glUseProgram(renderer->shader);
}

void
renderer_render(struct renderer *renderer)
{
    u32 cap, len;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!renderer->sprites)
    {
        return;
    }

    len = array__len(renderer->sprites);
    cap = array__cap(renderer->sprites);

    glBindVertexArray(renderer->vao);

    // TODO: Group by shader & texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->states[0].texture);

    glUseProgram(renderer->states[0].shader);

    // Buffer Orphaning
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[1]);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(struct renderer_sprite) * cap,
                 NULL,
                 GL_STREAM_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    len * sizeof(struct renderer_sprite),
                    renderer->sprites);

    // TODO: Get Matrices

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);
}
