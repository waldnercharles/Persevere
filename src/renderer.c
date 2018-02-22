#include "renderer.h"
#include "fbo.h"
#include "array.h"
#include "shader.h"

#include "stb_image.h"

void
renderer_init_shadow_caster_buffer(struct renderer *renderer)
{
    glGenBuffers(1, &(renderer->vbo.shadow_caster));
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.shadow_caster);

    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
renderer_update_shadow_caster_buffer(struct renderer *renderer)
{
    static const struct renderer_basic triangle[] = {
        {
            .pos = {.x = -0.5f, .y = 0.5f },
            .color = {.r = 1.0f, .g = 0.0f, .b = 0.0f },
        },
        {
            .pos = {.x = -0.5f, .y = -0.5f },
            .color = {.r = 1.0f, .g = 0.0f, .b = 0.0f },
        },
        {
            .pos = {.x = 0.5f, .y = -0.5f },
            .color = {.r = 1.0f, .g = 0.0f, .b = 0.0f },
        },
    };

    // static const r32 triangle[] = {
    //     -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // };

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.shadow_caster);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
renderer_init_light_buffer(struct renderer *renderer)
{
    glGenBuffers(1, &(renderer->vbo.light));
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.light);

    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
renderer_update_light_buffer(struct renderer *renderer,
                             v2 pos,
                             v2 size,
                             v3 color,
                             r32 intensity)
{
    r32 buffer[] = { pos.x,   pos.y,   size.x,  size.y,
                     color.r, color.g, color.b, intensity };

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.light);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
renderer_init_light_vao(struct renderer *renderer)
{
    // pos + size + color
    u32 size = sizeof(v2) + sizeof(v2) + sizeof(v3) + sizeof(r32);
    u32 pos_offset = 0;
    u32 size_offset = pos_offset + sizeof(v2);
    u32 color_offset = size_offset + sizeof(v2);
    u32 intensity_offset = color_offset + sizeof(v3);

    glGenVertexArrays(1, &(renderer->vao.light));
    glBindVertexArray(renderer->vao.light);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.quad);

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 2 * sizeof(r32), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.light);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, size, (void *)pos_offset);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, size, (void *)size_offset);
    glVertexAttribPointer(3, 3, GL_FLOAT, 0, size, (void *)color_offset);
    glVertexAttribPointer(4, 1, GL_FLOAT, 0, size, (void *)intensity_offset);

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
renderer_init_shadow_caster_vao(struct renderer *renderer)
{
    u32 size = sizeof(struct renderer_basic);
    u32 pos_offset = offsetof(struct renderer_basic, pos);
    u32 color_offset = offsetof(struct renderer_basic, color);

    glGenVertexArrays(1, &(renderer->vao.shadow_caster));
    glBindVertexArray(renderer->vao.shadow_caster);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.shadow_caster);

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, size, (void *)pos_offset);
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, size, (void *)color_offset);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
renderer_init(struct renderer *renderer, struct allocator *allocator)
{
    renderer->fbo = alloc(allocator, sizeof(struct fbo));
    renderer->sprite_renderer =
        alloc(allocator, sizeof(struct sprite_renderer));

    // TODO: Do not hardcode the framebuffer size
    fbo_init(renderer->fbo, 256, 256);

    // We render 2d, no need for face-culling
    glDisable(GL_CULL_FACE);

    // TODO: Get Uniforms

    // renderer_fbo_init(renderer);
    sprite_renderer_init(renderer->sprite_renderer, allocator);

    renderer_init_light_buffer(renderer);
    renderer_init_shadow_caster_buffer(renderer);

    renderer_init_light_vao(renderer);
    renderer_init_shadow_caster_vao(renderer);
}

// struct shadowfin
// {
//     v2 pos;

//     v2 outer;
//     v2 inner;

//     r32 outer_intensity;
//     r32 inner_intensity;
//     r32 depth;
// };

// struct shadowfin
// shadowfin_init(struct light light, v2 pos)
// {
//     (void)light;
//     struct shadowfin fin = { 0 };

//     fin.pos = pos;
//     fin.outer_intensity = 1.0f;
//     fin.inner_intensity = 0.0f;
//     fin.depth = 0.0f;

//     return fin;
// }

// void
// create_shadowfins(v2 points[], u32 start_idx, s32 direction)
// {
//     u32 i;
//     v2 p0, p1;
//     v2 edge;

//     struct shadowfin shadowfin;
//     r32 angle;

//     i = start_idx;

//     while (true)
//     {
//         p1 = points[i];
//         i = (i - step + 3) % 3;
//         p0 = points[0];

//         edge = vec2_sub(p1, p0);
//         edge = vec2_norm(edge);

//         penumbra = create_shadowfin(p0);
//         angle = atan2f(edge.y, edge.x) -
//     }
// }

void
render_shadows(v2 points[], v2 light_pos)
{
    u32 i;
    u32 curr_idx, prev_idx;
    s32 first_idx, last_idx;

    v2 curr, prev;
    v2 normal_vector, light_vector;
    bool prev_frontfacing = false;

    first_idx = last_idx = -1;

    for (i = 0; i < 3; i++)
    {
        curr_idx = i;
        prev_idx = (i + 2) % 3;

        curr = points[curr_idx];
        prev = points[prev_idx];

        normal_vector = vec2(-(curr.y - prev.y), curr.x - prev.x);
        light_vector = vec2(curr.x - light_pos.x, curr.y - light_pos.y);

        // Check if front-facing
        if (vec2_dot(normal_vector, light_vector) > 0)
        {
            if (!prev_frontfacing)
            {
                last_idx = prev_idx;
            }
            prev_frontfacing = true;
        }
        else
        {
            if (prev_frontfacing)
            {
                first_idx = prev_idx;
            }
            prev_frontfacing = false;
        }
    }

    if (first_idx == -1 || last_idx == -1)
    {
        return;
    }
}

void
renderer_render(struct renderer *renderer)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    sprite_renderer_render(renderer->sprite_renderer);

    // renderer_update_shadow_caster_buffer(renderer);

    // // TODO: Get Matrices

    // fbo_enable(renderer->fbo);

    // glDepthFunc(GL_LEQUAL);
    // glClearDepth(1.0f);

    // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
    // GL_STENCIL_BUFFER_BIT);

    // glBindTexture(GL_TEXTURE_2D, 0);

    // // Fill z-buffer
    // {
    //     glEnable(GL_DEPTH_TEST);
    //     glDepthMask(true);
    //     glColorMask(0, 0, 0, 0);

    //     glBindVertexArray(renderer->vao.shadow_caster);
    //     glUseProgram(renderer->shader.basic);
    //     glDrawArrays(GL_TRIANGLES, 0, 3);

    //     glDepthMask(false);
    //     glDisable(GL_DEPTH_TEST);
    // }

    // // TODO: For each light!
    // {
    //     r32 light_radius = 2.0f;
    //     v2 light_pos, light_size;
    //     v3 light_color;

    //     light_pos = vec2(0.3f, 0.6f);
    //     light_size = vec2(light_radius, light_radius);
    //     light_color = vec3(1.0f, 1.0f, 1.0f);

    //     // TODO: Update this with params from light
    //     renderer_update_light_buffer(renderer,
    //                                  light_pos,
    //                                  light_size,
    //                                  light_color,
    //                                  1.0f);

    //     // Clear framebuffer alpha
    //     {
    //         glColorMask(0, 0, 0, 1);
    //         glClear(GL_COLOR_BUFFER_BIT);
    //     }

    //     // Write framebuffer alpha
    //     {
    //         glEnable(GL_DEPTH_TEST);
    //         glDepthFunc(GL_LEQUAL);
    //         glDisable(GL_BLEND);
    //         glColorMask(1, 1, 1, 1);

    //         glBindVertexArray(renderer->vao.light);
    //         glUseProgram(renderer->shader.light);
    //         // TODO: This is kind of weird. Remove instanced rendering here
    //         glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

    //         glEnable(GL_BLEND);
    //         glBlendFunc(GL_DST_ALPHA, GL_ZERO);

    //         // TODO: For every shadow caster, create shadow geometry
    //         v2 points[] = { vec2(-0.5f, +0.5f),
    //                         vec2(-0.5f, -0.5f),
    //                         vec2(+0.5f, -0.5f) };
    //         (void)points;

    //         // render_shadows(points, light_pos);

    //         glDisable(GL_DEPTH_TEST);
    //     }

    //     // Draw Geometry
    //     {
    //         glEnable(GL_DEPTH_TEST);
    //         glEnable(GL_BLEND);
    //         glBlendFunc(GL_DST_ALPHA, GL_ONE);
    //         glColorMask(1, 1, 1, 0);

    //         // u32 len = array_count(renderer->sprites);
    //         // glBindVertexArray(renderer->vao.geometry);
    //         // glActiveTexture(GL_TEXTURE0);
    //         // glBindTexture(GL_TEXTURE_2D, renderer->states[0].texture);

    //         // glUseProgram(renderer->states[0].shader);
    //         // glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, len);

    //         glBindVertexArray(renderer->vao.light);
    //         glUseProgram(renderer->shader.light);
    //         glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

    //         glBindVertexArray(renderer->vao.shadow_caster);
    //         glUseProgram(renderer->shader.basic);
    //         glDrawArrays(GL_TRIANGLES, 0, 3);
    //     }
    // }

    // fbo_disable(renderer->fbo);

    // glDisable(GL_DEPTH_TEST);
    // glDisable(GL_BLEND);

    // // TODO: Render the fbo
    // fbo_render(renderer, renderer->fbo);
}
