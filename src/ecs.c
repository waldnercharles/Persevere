#include "ecs.h"

#include "array.h"
#include "log.h"

#include "systems.h"
#include "components.h"

void *
ecs__entity_get_data(struct ecs *ecs, u32 entity)
{
    return (void *)(array_get(ecs->data, entity));
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

    array_alloc(ecs->allocator, 0, ecs->data_width, &(ecs->data));

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
        if (system->process_begin != NULL)
        {
            system->process_begin(ecs, u_data);
        }

        bitset_for_each (i, &(system->entities))
        {
            system->process(ecs, u_data, i, dt);
        }

        if (system->process_end != NULL)
        {
            system->process_end(ecs, u_data);
        }
    }
}

// component
void
ecs_register_component(struct ecs *ecs, char *name, u32 size, u32 *id)
{
    struct ecs_component c;

    c = (struct ecs_component){
        c.id = ecs->num_components,
        c.name = name,
        c.size = size,
        c.offset = ecs->data_width,
    };

    ecs->data_width += size;

    if (ecs->components == NULL)
    {
        array_alloc(ecs->allocator, 0, sizeof(c), &(ecs->components));
    }

    array_grow_to(ecs->components, c.id + 1);

    log_debug("Registering component (%s, %i).", c.name, c.id);
    array_set(ecs->components, c.id, &c);
    ++(ecs->components->len);
    ++(ecs->num_components);

    *id = c.id;
}

// system
void
ecs_register_system(struct ecs *ecs,
                    char *name,
                    struct ecs_system_funcs funcs,
                    u32 *id)
{
    struct ecs_system s;

    s = (struct ecs_system){
        .id = ecs->num_systems,
        .name = name,
        .process_begin = funcs.process_begin,
        .process = funcs.process,
        .process_end = funcs.process_end,
    };

    bitset_init(&s.entities, ecs->allocator);
    array_alloc(ecs->allocator, 0, sizeof(u32), &(s.watched_components));

    if (ecs->systems == NULL)
    {
        array_alloc(ecs->allocator,
                    0,
                    sizeof(struct ecs_system),
                    &(ecs->systems));
    }

    array_grow_to(ecs->systems, s.id + 1);

    log_debug("Registering system (%s, %i)", s.name, s.id);
    array_set(ecs->systems, s.id, &s);
    ++(ecs->systems->len);
    ++(ecs->num_systems);

    *id = s.id;
}

void
ecs_watch(struct ecs *ecs, u32 system_id, u32 component_id)
{
    struct ecs_system *system;

    system = array_get(ecs->systems, system_id);
    array_push(system->watched_components, &component_id);
}

void
ecs_process_system(struct ecs *ecs, u32 system_id, void *u_data, r32 dt)
{
    struct ecs_system *system;
    u32 entity;

    system = array_get(ecs->systems, system_id);
    bitset_for_each (entity, &system->entities)
    {
        system->process(ecs, u_data, entity, dt);
    }
}

// entity
void
ecs_create_entity(struct ecs *ecs, u32 *entity_ptr)
{
    u32 id = sparse_set_is_empty(&ecs->free_entities)
                 ? ecs->next_entity_id++
                 : sparse_set_pop(&ecs->free_entities);

    u32 cap = ecs->data->cap;

    if (cap < ecs->next_entity_id)
    {
        array_grow_to_at_least(ecs->data, ecs->next_entity_id);
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
                  u32 entity_id,
                  u32 component_id,
                  void **component_data_ptr)
{
    u8 *entity_data;
    struct ecs_component *component;

    entity_data = ecs__entity_get_data(ecs, entity_id);
    component = array_get(ecs->components, component_id);

    // Component not found
    if (!bitset_test(entity_data, component_id))
    {
        return;
    }

    *component_data_ptr = (void *)(entity_data + component->offset);
}

void
ecs_set_component(struct ecs *ecs,
                  u32 entity_id,
                  u32 component_id,
                  void *component_data)
{
    u8 *entity_data;
    struct ecs_component *component;

    entity_data = ecs__entity_get_data(ecs, entity_id);
    component = array_get(ecs->components, component_id);

    bitset_set(entity_data, component_id);
    memcpy((entity_data + component->offset), component_data, component->size);
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
