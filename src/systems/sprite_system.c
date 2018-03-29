#include "systems/sprite_system.h"

#include "std.h"
#include "engine.h"
#include "array.h"
#include "sort.h"
#include "vec.h"
#include "renderer.h"
#include "renderers/sprite_renderer.h"

#include "components.h"

void
sprite_system_process_begin(struct ecs *ecs, void *u_data)
{
    struct engine *engine;
    struct sprite_renderer *sprite_renderer;
    unused(ecs);

    engine = u_data;

    sprite_renderer = engine->renderer->sprite_renderer;
    array__len(sprite_renderer->sprites) = 0;
    array__len(sprite_renderer->shaders) = 0;
    array__len(sprite_renderer->textures) = 0;
}

void
sprite_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    struct components *components = ecs->component_handles;

    struct position *position;
    struct quad *quad;
    struct material *material;

    struct sprite_vertex vertex;

    struct engine *engine;
    struct sprite_renderer *renderer;

    unused(dt);

    engine = u_data;
    renderer = engine->renderer->sprite_renderer;

    ecs_get_component(ecs, entity, components->position, (void **)&position);
    ecs_get_component(ecs, entity, components->quad, (void **)&quad);
    ecs_get_component(ecs, entity, components->material, (void **)&material);

    vertex.pos = position->pos;
    vertex.size = quad->size;
    vertex.uv = material->uv_offset;

    array_push(renderer->sprites, vertex);
    array_push(renderer->shaders, material->shader);
    array_push(renderer->textures, material->texture);
}

s32
sprite_comparator(const void *left, const void *right, void *u_data)
{
    s32 left_idx, right_idx;
    r32 z_cmp;
    struct sprite_renderer *renderer = u_data;

    left_idx = *(s32 *)left;
    right_idx = *(s32 *)right;

    z_cmp =
        renderer->sprites[left_idx].pos.z - renderer->sprites[right_idx].pos.z;

    if (z_cmp > 0)
    {
        return -1;
    }
    if (z_cmp < 0)
    {
        return 1;
    }

    if (renderer->shaders[left_idx] != renderer->shaders[right_idx])
    {
        return 1;
    }
    if (renderer->shaders[left_idx] != renderer->shaders[right_idx])
    {
        return 1;
    }

    return 0;
}

void
sprite_system_process_end(struct ecs *ecs, void *u_data)
{
    struct engine *engine;
    struct sprite_renderer *renderer;

    s32 i, len;
    s32 tmp_idx;
    struct sprite_vertex tmp_sprite;
    u32 tmp_shader;
    u32 tmp_texture;

    unused(ecs);

    engine = u_data;
    renderer = engine->renderer->sprite_renderer;

    len = array_count(renderer->sprites);
    s32 index[len];

    for (i = 0; i < len; ++i)
    {
        index[i] = i;
    }

    heap_sort(index, len, sizeof(s32), renderer, sprite_comparator);

    for (i = 0; i < len; ++i)
    {
        while (index[i] != i)
        {
            // swap sprite
            tmp_sprite = renderer->sprites[index[i]];
            renderer->sprites[index[i]] = renderer->sprites[i];
            renderer->sprites[i] = tmp_sprite;

            // swap shader
            tmp_shader = renderer->shaders[index[i]];
            renderer->shaders[index[i]] = renderer->shaders[i];
            renderer->shaders[i] = tmp_shader;

            // swap textures
            tmp_texture = renderer->textures[index[i]];
            renderer->textures[index[i]] = renderer->textures[i];
            renderer->textures[i] = tmp_texture;

            // swap index
            tmp_idx = index[index[i]];
            index[index[i]] = index[i];
            index[i] = tmp_idx;
        }
    }
}