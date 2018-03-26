#include "systems/light_system.h"

#include "array.h"
#include "ecs.h"
#include "engine.h"

#include "components.h"
#include "renderers/light_renderer.h"

void
light_system_process_begin(struct ecs *ecs, void *u_data)
{
    struct engine *engine;
    struct light_renderer *renderer;

    unused(ecs);

    engine = u_data;

    renderer = engine->renderer->light_renderer;
    array__len(renderer->lights) = 0;
}

void
light_system_process(struct ecs *ecs, void *u_data, u32 entity, r32 dt)
{
    struct components *components = ecs->component_handles;

    v2 *position;
    struct light *light;

    struct light_vertex vertex;

    struct engine *engine;
    struct light_renderer *renderer;

    unused(dt);

    engine = u_data;
    renderer = engine->renderer->light_renderer;

    ecs_get_component(ecs, entity, components->position, (void **)&position);
    ecs_get_component(ecs, entity, components->light, (void **)&light);

    vertex.pos = *position;

    vertex.size = vec2(light->light_radius, light->light_radius);

    vertex.color = light->color;
    vertex.intensity = light->intensity;

    array_push(renderer->lights, vertex);
}