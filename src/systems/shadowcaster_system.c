#include "systems/shadowcaster_system.h"

#include "array.h"
#include "ecs.h"
#include "engine.h"

#include "renderers/shadowcaster_renderer.h"

void
shadowcaster_system_process_begin(struct ecs *ecs, void *u_data)
{
    struct engine *engine;
    struct shadowcaster_renderer *renderer;

    unused(ecs);

    engine = u_data;
    renderer = engine->renderer->shadowcaster_renderer;

    array__len(renderer->casters) = 0;
}

void
shadowcaster_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    struct components *components = ecs->component_handles;

    v2 *position;
    struct shadowcaster *shadowcaster;

    struct shadowcaster_vertex vertex;

    struct engine *engine;
    struct shadowcaster_renderer *renderer;

    unused(dt);

    engine = u_data;
    renderer = engine->renderer->shadowcaster_renderer;

    ecs_get_component(ecs, entity, components->position, (void **)&position);
    ecs_get_component(ecs,
                      entity,
                      components->shadowcaster,
                      (void **)&shadowcaster);

    vertex.pos = *position;
    vertex.size = shadowcaster->size;

    array_push(renderer->casters, vertex);
}
