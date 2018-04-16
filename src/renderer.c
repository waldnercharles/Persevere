#include "renderer.h"
#include "fbo.h"
#include "array.h"
#include "shader.h"

#include "stb_image.h"

void
renderer_init_shadowcaster_buffer(struct renderer *renderer)
{
    glGenBuffers(1, &(renderer->vbo.shadowcaster));
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.shadowcaster);

    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
renderer_update_shadowcaster_buffer(struct renderer *renderer)
{
    static const struct renderer_basic triangle[] = {
        {
            .pos = { .x = -0.5f, .y = 0.5f },
            .color = { .r = 1.0f, .g = 0.0f, .b = 0.0f },
        },
        {
            .pos = { .x = -0.5f, .y = -0.5f },
            .color = { .r = 1.0f, .g = 0.0f, .b = 0.0f },
        },
        {
            .pos = { .x = 0.5f, .y = -0.5f },
            .color = { .r = 1.0f, .g = 0.0f, .b = 0.0f },
        },
    };

    // static const r32 triangle[] = {
    //     -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // };

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.shadowcaster);
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
    static const u64 size = sizeof(v2) + sizeof(v2) + sizeof(v3) + sizeof(r32);
    static const u64 pos_offset = 0;
    static const u64 size_offset = pos_offset + sizeof(v2);
    static const u64 color_offset = size_offset + sizeof(v2);
    static const u64 intensity_offset = color_offset + sizeof(v3);

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
renderer_init_shadowcaster_vao(struct renderer *renderer)
{
    static const u64 size = sizeof(struct renderer_basic);
    static const u64 pos_offset = offsetof(struct renderer_basic, pos);
    static const u64 color_offset = offsetof(struct renderer_basic, color);

    glGenVertexArrays(1, &(renderer->vao.shadowcaster));
    glBindVertexArray(renderer->vao.shadowcaster);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo.shadowcaster);

    glVertexAttribPointer(0, 2, GL_FLOAT, 0, size, (void *)pos_offset);
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, size, (void *)color_offset);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
renderer_init(struct renderer *renderer,
              struct engine *engine,
              struct allocator *allocator)
{
    u32 *fbo_frag, *fbo_vert;

    renderer->fbo = alloc(allocator, sizeof(struct fbo));

    renderer->sprite_renderer =
        alloc(allocator, sizeof(struct sprite_renderer));

    renderer->shadowcaster_renderer =
        alloc(allocator, sizeof(struct shadowcaster_renderer));

    renderer->light_renderer = alloc(allocator, sizeof(struct light_renderer));

    // TODO: Do not hardcode the framebuffer size

    fbo_frag = asset_get(engine->assets, "assets/fbo.frag");
    fbo_vert = asset_get(engine->assets, "assets/fbo.vert");
    fbo_init(renderer->fbo, 512, 512);
    renderer->fbo->shader = shader_program_link(*fbo_frag, *fbo_vert);

    // We render 2d, no need for face-culling
    glDisable(GL_CULL_FACE);

    // TODO: Get Uniforms

    // renderer_fbo_init(renderer);
    sprite_renderer_init(renderer->sprite_renderer, allocator);
    light_renderer_init(renderer->light_renderer, engine, allocator);

    shadowcaster_renderer_init(engine,
                               renderer->shadowcaster_renderer,
                               allocator);

    renderer_init_light_buffer(renderer);
    renderer_init_shadowcaster_buffer(renderer);

    renderer_init_light_vao(renderer);
    renderer_init_shadowcaster_vao(renderer);
}

struct shadowfin
{
    u32 index;

    v2 pos;

    v2 outer;
    v2 inner;

    r32 outer_intensity;
    r32 inner_intensity;
    r32 depth;
};

struct shadowfin
create_shadowfin(v2 pos)
{
    return (struct shadowfin){
        .pos = pos,
        .outer = vec2(0.0f, 0.0f),
        .inner = vec2(0.0f, 0.0f),
        .outer_intensity = 1.0f,
        .inner_intensity = 0.0f,
        .depth = 0.0f,
        .index = 0,
    };
}

v2
get_shadow_vector(v2 pos, r32 radius, v2 edge, s32 step, b32 inner)
{
    v2 center;
    v2 perp;
    b32 invert;
    r32 rotate_direction;

    invert = false;
    if (pos.x < edge.x)
    {
        invert = true;
    }

    rotate_direction = inner ? -1.0f : 1.0f;

    perp = vec2_sub(pos, edge);
    perp = vec2_norm(perp);
    if (step == 1)
    {
        if (invert)
        {
            perp = vec2_mul(perp, -radius);
            perp = vec2_rotate(perp, MATH_PI * 0.5f * rotate_direction);
        }
        else
        {
            perp = vec2_mul(perp, radius);
            perp = vec2_rotate(perp, -MATH_PI * 0.5f * rotate_direction);
        }
    }
    else
    {
        if (invert)
        {
            perp = vec2_mul(perp, -radius);
            perp = vec2_rotate(perp, -MATH_PI * 0.5f * rotate_direction);
        }
        else
        {
            perp = vec2_mul(perp, radius);
            perp = vec2_rotate(perp, MATH_PI * 0.5f * rotate_direction);
        }
    }

    center = vec2_mul(vec2_sub(vec2_add(pos, perp), edge), -1);
    center = vec2_norm(center);

    return vec2_mul(center, radius * 10.0f);
}

v2
get_outer_vector(v2 pos, r32 radius, v2 edge, s32 step)
{
    return get_shadow_vector(pos, radius, edge, step, false);
}

v2
get_inner_vector(v2 pos, r32 radius, v2 edge, s32 step)
{
    return get_shadow_vector(pos, radius, edge, step, true);
}

void
create_shadowfins(v2 points[],
                  struct light_vertex light,
                  u32 start_idx,
                  s32 step,
                  struct shadowfin *shadowfins)
{
    unused(points);
    unused(light);
    unused(start_idx);
    unused(step);
    unused(shadowfins);

    // u32 i;
    // v2 p0, p1;
    // v2 edge;

    // struct shadowfin shadowfin;
    // v2 outer, inner;
    // r32 angle;

    // i = start_idx;

    // while (true)
    // {
    //     p1 = points[i];

    //     i = (i - step + 3) % 4;

    //     p0 = points[i];

    //     edge = vec2_sub(p1, p0);
    //     edge = vec2_norm(edge);

    //     shadowfin = create_shadowfin(p0);
    //     shadowfin.index = i;

    //     outer = get_outer_vector(light.pos, 0.1f, p0, step);
    //     inner = get_inner_vector(light.pos, 0.1f, p0, step);

    //     angle = atan2f(edge.y, edge.x) - atan2f(outer.y, outer.x);

    //     if (step == 1)
    //     {
    //         if (angle < 0 || angle > MATH_PI * 0.5f)
    //         {
    //             break;
    //         }
    //     }
    //     else if (step == -1)
    //     {
    //         if (angle > MATH_PI)
    //         {
    //             angle -= MATH_PI * 2.0f;
    //         }
    //         if (angle > 0 || angle < -MATH_PI * 0.5f)
    //         {
    //             break;
    //         }
    //     }

    //     shadowfin.outer = outer;
    //     shadowfin.inner = vec2_mul(edge, vec2_mag(inner));

    //     array_push(shadowfins, shadowfin);
    // }
}

void
render_shadows(v2 points[], v2 light_pos)
{
    u32 i;
    u32 curr_idx, prev_idx;
    s32 start_idx, end_idx;

    v2 curr, prev;
    v2 normal_vector, light_vector;
    b32 prev_frontfacing = false;

    start_idx = end_idx = -1;

    for (i = 0; i < 4; ++i)
    {
        prev_idx = i;
        curr_idx = (i + 1) % 4;

        curr = points[curr_idx];
        prev = points[prev_idx];

        normal_vector = vec2(-(curr.y - prev.y), curr.x - prev.x);
        light_vector = vec2(curr.x - light_pos.x, curr.y - light_pos.y);

        // Check if facing light
        if (vec2_dot(normal_vector, light_vector) > 0)
        {
            if (!prev_frontfacing)
            {
                end_idx = prev_idx;
            }
            prev_frontfacing = true;
        }
        else
        {
            if (prev_frontfacing)
            {
                start_idx = prev_idx;
            }
            prev_frontfacing = false;
        }
    }

    if (start_idx == -1 || end_idx == -1)
    {
        return;
    }
}

void
renderer_render(struct renderer *renderer)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    fbo_enable(renderer->fbo);
    sprite_renderer_render(renderer->sprite_renderer);
    fbo_disable(renderer->fbo);

    fbo_render(renderer->fbo);
}
