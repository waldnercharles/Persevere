#include "systems/sprite_system.h"

#include "std.h"
#include "engine.h"
#include "array.h"
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
    v2 *position;
    struct quad *quad;
    struct material *material;

    struct sprite_vertex vertex;

    struct engine *engine;
    struct sprite_renderer *renderer;

    unused(dt);

    engine = u_data;
    renderer = engine->renderer->sprite_renderer;

    ecs_get_component(ecs, entity, component_position_id, (void **)&position);
    ecs_get_component(ecs, entity, component_quad_id, (void **)&quad);
    ecs_get_component(ecs, entity, component_material_id, (void **)&material);

    vertex.pos = *position;
    vertex.size = quad->size;
    vertex.uv = material->uv_offset;

    array_push(renderer->sprites, vertex);
    array_push(renderer->shaders, material->shader);
    array_push(renderer->textures, material->texture);
}