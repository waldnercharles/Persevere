#include "ecs.h"

#include "array.h"
#include "log.h"

#include "systems.h"
#include "components.h"

void *
ecs__entity_get_data(struct ecs *ecs, u32 entity)
{
    return (void *)((u8 *)ecs->data + (ecs->data_width * entity));
}

void
ecs__subscribe(struct ecs_system *system, u32 entity)
{
    bitset_insert(&system->entities, entity);
}

void
ecs__unsubscribe(struct ecs_system *system, u32 entity)
{
    bitset_delete(&system->entities, entity);
}

static void
ecs__check(struct ecs *ecs, struct ecs_system *system, u32 entity)
{
    u32 *component;
    u8 *entity_components = ecs__entity_get_data(ecs, entity);

    array_for_each (component, system->watched_components)
    {
        if (!bitset_test(entity_components, *component))
        {
            ecs__unsubscribe(system, entity);
            return;
        }
    }
    ecs__subscribe(system, entity);
}

void
ecs_init(struct ecs *ecs, struct allocator *allocator)
{
    ecs->allocator = allocator;
    sparse_set_init(&ecs->added_entities, ecs->allocator);
    sparse_set_init(&ecs->enabled_entities, ecs->allocator);
    sparse_set_init(&ecs->deleted_entities, ecs->allocator);
    sparse_set_init(&ecs->disabled_entities, ecs->allocator);
    sparse_set_init(&ecs->dirty_entities, ecs->allocator);

    sparse_set_init(&ecs->free_entities, ecs->allocator);

    bitset_init(&ecs->active_entities, ecs->allocator);
}

void
ecs_finalize(struct ecs *ecs)
{
    struct ecs_component *component;
    u32 component_bytes = bitset_nslots(ecs->num_components);

    ecs->data_width += component_bytes;
    array_for_each (component, ecs->components)
    {
        component->offset += component_bytes;
    }

    ecs->data = array__init(ecs->allocator, 0, ecs->data_width);

    ecs->initialized = 1;
}

void
ecs_process(struct ecs *ecs, void *u_data, r32 dt)
{
    u32 entity, i, j;
    struct ecs_system *system;

    if (!ecs->initialized)
    {
        log_error("Entity component system not initialized.");
    }

    // TODO: Handle added event
    sparse_set_clear(&ecs->added_entities);

    // enabled entities
    sparse_set_for_each (entity, i, &ecs->enabled_entities)
    {
        array_for_each (system, ecs->systems)
        {
            ecs__check(ecs, system, entity);
        }
        // TODO: Handle enabled event
        bitset_insert(&ecs->active_entities, entity);
    }
    sparse_set_clear(&ecs->enabled_entities);

    // disabled entities
    sparse_set_for_each (entity, i, &ecs->disabled_entities)
    {
        array_for_each (system, ecs->systems)
        {
            ecs__unsubscribe(system, entity);
        }
        // TODO: Handle disabled event
        bitset_delete(&ecs->active_entities, entity);
    }
    sparse_set_clear(&ecs->disabled_entities);

    // deleted entities
    sparse_set_for_each (entity, i, &ecs->deleted_entities)
    {
        array_for_each (system, ecs->systems)
        {
            ecs__unsubscribe(system, entity);
        }
        // TODO: Handle deleted event
        for (j = 0; j < ecs->num_components; ++j)
        {
            ecs_rem_component(ecs, entity, j);
        }
        sparse_set_insert(&ecs->free_entities, entity);
    }
    sparse_set_clear(&ecs->deleted_entities);

    // systems
    array_for_each (system, ecs->systems)
    {
        if (system_functions[system->id].process_begin != NULL)
        {
            system_functions[system->id].process_begin(ecs, u_data);
        }

        bitset_for_each (i, &(system->entities))
        {
            system_functions[system->id].process(ecs, u_data, i, dt);
        }

        if (system_functions[system->id].process_end != NULL)
        {
            system_functions[system->id].process_end(ecs, u_data);
        }
    }
}

// component
void
ecs_register_component(struct ecs *ecs, char *name, u32 size, u32 id)
{
    struct ecs_component tmp, c = { 0 };
    bool registered;

    c.id = id;
    c.name = name;
    c.size = size;
    c.offset = ecs->data_width;

    ecs->data_width += size;

    if (ecs->components == NULL)
    {
        array_init(ecs->components, ecs->allocator);
    }

    array__grow_to(ecs->components, id + 1);
    tmp = ecs->components[id];

    registered = tmp.id == c.id && tmp.size == c.size &&
                 tmp.offset == c.offset && tmp.name != NULL &&
                 strcmp(tmp.name, c.name) == 0;

    if (registered)
    {
        log_warning("Component (%s, %i) already registered.", c.name, c.id);
        return;
    }
    else
    {
        log_debug("Registering component (%s, %i).", c.name, c.id);
        ecs->components[id] = c;
        ++(array__len(ecs->components));
        ++(ecs->num_components);
    }
}

// system
void
ecs_register_system(struct ecs *ecs, char *name, u32 id)
{
    struct ecs_system tmp, s = { 0 };
    bool registered;

    bitset_init(&s.entities, ecs->allocator);

    s.id = id;
    s.name = name;
    s.entities.bytes = NULL;
    s.entities.capacity = 0;

    array_init(s.watched_components, ecs->allocator);

    if (ecs->systems == NULL)
    {
        array_init(ecs->systems, ecs->allocator);
    }

    array__grow_to(ecs->systems, id + 1);
    tmp = ecs->systems[id];

    registered =
        tmp.id == s.id && tmp.name != NULL && strcmp(tmp.name, s.name) == 0;

    if (registered)
    {
        log_warning("System (%s, %i) already registered", s.name, s.id);
    }
    else
    {
        log_debug("Registering system (%s, %i)", s.name, s.id);
        ecs->systems[id] = s;
        ++(array__len(ecs->systems));
        ++(ecs->num_systems);
    }
}

void
ecs_watch(struct ecs *ecs, u32 system, u32 component)
{
    array_push(ecs->systems[system].watched_components, component);
}

void
ecs_process_system(struct ecs *ecs, u32 system, void *u_data, r32 dt)
{
    struct ecs_system *s = &ecs->systems[system];
    u32 entity;

    bitset_for_each (entity, &s->entities)
    {
        system_functions[s->id].process(ecs, u_data, entity, dt);
    }
}

// entity
void
ecs_create_entity(struct ecs *ecs, u32 *entity_ptr)
{
    u32 id = sparse_set_is_empty(&ecs->free_entities)
                 ? ecs->next_entity_id++
                 : sparse_set_pop(&ecs->free_entities);

    u32 cap = array__cap(ecs->data);

    if (cap < ecs->next_entity_id)
    {
        array__grow(ecs->data, ecs->next_entity_id - cap);
    }

    *entity_ptr = id;
}

void
ecs_clone_entity(struct ecs *ecs, u32 entity, u32 *entity_ptr)
{
    (void)ecs, (void)entity, (void)entity_ptr;
    // TODO
}
void
ecs_get_component(struct ecs *ecs,
                  u32 entity,
                  u32 component,
                  void **component_data_ptr)
{
    u8 *entity_data = ecs__entity_get_data(ecs, entity);
    struct ecs_component *component_info = &(ecs->components[component]);

    // Component not found
    if (!bitset_test(entity_data, component))
    {
        return;
    }

    *component_data_ptr = (void *)(entity_data + component_info->offset);
}

void
ecs_set_component(struct ecs *ecs,
                  u32 entity,
                  u32 component,
                  void *component_data)
{
    u8 *entity_data = ecs__entity_get_data(ecs, entity);
    struct ecs_component *component_info = &(ecs->components[component]);

    bitset_set(entity_data, component);

    memcpy((entity_data + component_info->offset),
           component_data,
           component_info->size);
}

void
ecs_rem_component(struct ecs *ecs, u32 entity, u32 component)
{
    u8 *entity_data = ecs__entity_get_data(ecs, entity);
    bitset_clear(entity_data, component);
}

void
ecs_set_state(struct ecs *ecs, u32 entity, enum ecs_state state)
{
    switch (state)
    {
        case ECS_STATE_ADDED:
            sparse_set_insert(&ecs->added_entities, entity);
            sparse_set_insert(&ecs->enabled_entities, entity);
            sparse_set_delete(&ecs->disabled_entities, entity);
            sparse_set_delete(&ecs->deleted_entities, entity);
            break;
        case ECS_STATE_ENABLED:
            sparse_set_insert(&ecs->enabled_entities, entity);
            sparse_set_delete(&ecs->disabled_entities, entity);
            sparse_set_delete(&ecs->deleted_entities, entity);
            break;
        case ECS_STATE_DISABLED:
            sparse_set_delete(&ecs->enabled_entities, entity);
            sparse_set_insert(&ecs->disabled_entities, entity);
            sparse_set_delete(&ecs->deleted_entities, entity);
            break;
        case ECS_STATE_DELETED:
            sparse_set_delete(&ecs->added_entities, entity);
            sparse_set_delete(&ecs->enabled_entities, entity);
            sparse_set_insert(&ecs->disabled_entities, entity);
            sparse_set_insert(&ecs->deleted_entities, entity);
            break;
    }
}
