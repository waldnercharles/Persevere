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
    sprite_renderer->sprites->len = 0;
    sprite_renderer->shaders->len = 0;
    sprite_renderer->textures->len = 0;
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

    array_push(renderer->sprites, &vertex);
    array_push(renderer->shaders, &(material->shader));
    array_push(renderer->textures, &(material->texture));
}

s32
sprite_comparator(const void *left, const void *right, void *u_data)
{
    struct sprite_renderer *renderer;
    s32 left_idx, right_idx;

    r32 z_cmp;

    u32 left_shader, right_shader;
    u32 left_texture, right_texture;
    struct sprite_vertex *left_sprite, *right_sprite;

    renderer = u_data;

    left_idx = *(s32 *)left;
    right_idx = *(s32 *)right;

    left_sprite = array_get(renderer->sprites, left_idx);
    right_sprite = array_get(renderer->sprites, right_idx);
    z_cmp = left_sprite->pos.z - right_sprite->pos.z;
    if (z_cmp > 0)
    {
        return 1;
    }
    if (z_cmp < 0)
    {
        return -1;
    }

    left_shader = *(u32 *)array_get(renderer->shaders, left_idx);
    right_shader = *(u32 *)array_get(renderer->shaders, right_idx);
    if (left_shader != right_shader)
    {
        return 1;
    }

    left_texture = *(u32 *)array_get(renderer->textures, left_idx);
    right_texture = *(u32 *)array_get(renderer->textures, right_idx);
    if (left_texture != right_texture)
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
    struct array *sprites, *shaders, *textures;

    s32 i, len;
    s32 tmp_idx;
    struct sprite_vertex tmp_sprite;
    u32 tmp_shader;
    u32 tmp_texture;

    unused(ecs);

    engine = u_data;
    renderer = engine->renderer->sprite_renderer;
    sprites = renderer->sprites;
    shaders = renderer->shaders;
    textures = renderer->textures;

    len = sprites->len;
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
            tmp_sprite = *(struct sprite_vertex *)array_get(sprites, index[i]);
            array_set(sprites, index[i], array_get(sprites, i));
            array_set(sprites, i, &tmp_sprite);

            // swap shader
            tmp_shader = *(u32 *)array_get(shaders, index[i]);
            array_set(shaders, index[i], array_get(shaders, i));
            array_set(shaders, i, &tmp_shader);

            // swap textures
            tmp_texture = *(u32 *)array_get(textures, index[i]);
            array_set(textures, index[i], array_get(textures, i));
            array_set(textures, i, &tmp_texture);

            // swap index
            tmp_idx = index[index[i]];
            index[index[i]] = index[i];
            index[i] = tmp_idx;
        }
    }
}