#include "ecs.h"
#include "array.h"

static void *
ecs__entity_get_data(struct ecs *ecs, u32 entity)
{
    return (void *)((u8 *)ecs->data + (ecs->data_width * entity));
}

static inline void
ecs__subscribe(struct ecs_system *system, u32 entity)
{
    bitset_insert(&system->entities, entity);
}

static inline void
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

struct ecs *
ecs_alloc()
{
    struct ecs *ecs = malloc(sizeof(struct ecs));
    memset(ecs, 0, sizeof(struct ecs));
    return ecs;
}

void
ecs_init(struct ecs *ecs)
{
    struct ecs_component *component;
    u32 component_bytes = bitset_nslots(ecs->num_components);

    ecs->data_width += component_bytes;
    array_for_each (component, ecs->components)
    {
        component->offset += component_bytes;
    }

    ecs->initialized = 1;
}

void
ecs_process(struct ecs *ecs, void *u_data, f32 dt)
{
    u32 entity, i, j;
    struct ecs_system *system;

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
        if (system->process_begin)
        {
            system->process_begin(ecs, u_data);
        }

        bitset_for_each (i, &(system->entities))
        {
            system->process(ecs, u_data, entity, dt);
        }

        if (system->process_end)
        {
            system->process_end(ecs, u_data);
        }
    }
}

// component
void
ecs_create_component(struct ecs *e, char *name, u32 size, u32 *component_ptr)
{
    struct ecs_component c;
    c.name = name;
    c.size = size;
    c.offset = e->data_width;

    e->data_width += size;

    array_push(e->components, c);
    *component_ptr = ++(e->num_components) - 1;
}

// system
void
ecs_create_system(struct ecs *ecs,
                  char *name,
                  void (*process_begin)(struct ecs *, void *),
                  void (*process)(struct ecs *, void *, u32, f32),
                  void (*process_end)(struct ecs *, void *),
                  u32 *system_ptr)
{
    struct ecs_system s;

    s.entities.bytes = NULL;
    s.entities.capacity = 0;

    s.name = name;
    s.process = process;
    s.process_begin = process_begin;
    s.process_end = process_end;

    s.watched_components = NULL;

    array_push(ecs->systems, s);
    *system_ptr = ++(ecs->num_systems) - 1;
}

void
ecs_watch(struct ecs *ecs, u32 system, u32 component)
{
    array_push(ecs->systems[system].watched_components, component);
}

void
ecs_process_system(struct ecs *ecs, u32 system, void *u_data, f32 dt)
{
    struct ecs_system *s = &ecs->systems[system];
    u32 entity;

    bitset_for_each (entity, &s->entities)
    {
        s->process(ecs, u_data, entity, dt);
    }
}

// entity
void
ecs_create_entity(struct ecs *ecs, u32 *entity_ptr)
{
    u32 id = sparse_set_is_empty(&ecs->free_entities)
                 ? ecs->next_entity_id++
                 : sparse_set_pop(&ecs->free_entities);

    array_set_cap_sz(ecs->data, ecs->next_entity_id, ecs->data_width);

    *entity_ptr = id;
}

void
ecs_clone_entity(struct ecs *ecs, u32 entity, u32 *entity_ptr)
{
    (void)ecs, (void)entity, (void)entity_ptr;
    // TODO
}
void
ecs_get_component(struct ecs *e,
                  u32 entity,
                  u32 component,
                  void **component_data_ptr)
{
    u8 *entity_data = ecs__entity_get_data(e, entity);
    struct ecs_component *component_info = &(e->components[component]);

    // Component not found
    if (!bitset_test(entity_data, component))
    {
        return;
    }

    *component_data_ptr = (void *)(entity_data + component_info->offset);
}

void
ecs_set_component(struct ecs *e,
                  u32 entity,
                  u32 component,
                  void *component_data)
{
    u8 *entity_data = ecs__entity_get_data(e, entity);
    struct ecs_component *component_info = &(e->components[component]);

    bitset_set(entity_data, component);

    memcpy((entity_data + component_info->offset),
           component_data,
           component_info->size);
}

void
ecs_rem_component(struct ecs *e, u32 entity, u32 component)
{
    u8 *entity_data = ecs__entity_get_data(e, entity);
    bitset_clear(entity_data, component);
}

void
ecs_set_state(struct ecs *e, u32 entity, enum ecs_state state)
{
    switch (state)
    {
        case ECS_STATE_ADDED:
            sparse_set_insert(&e->added_entities, entity);
            sparse_set_insert(&e->enabled_entities, entity);
            sparse_set_delete(&e->disabled_entities, entity);
            sparse_set_delete(&e->deleted_entities, entity);
            break;
        case ECS_STATE_ENABLED:
            sparse_set_insert(&e->enabled_entities, entity);
            sparse_set_delete(&e->disabled_entities, entity);
            sparse_set_delete(&e->deleted_entities, entity);
            break;
        case ECS_STATE_DISABLED:
            sparse_set_delete(&e->enabled_entities, entity);
            sparse_set_insert(&e->disabled_entities, entity);
            sparse_set_delete(&e->deleted_entities, entity);
            break;
        case ECS_STATE_DELETED:
            sparse_set_delete(&e->added_entities, entity);
            sparse_set_delete(&e->enabled_entities, entity);
            sparse_set_insert(&e->disabled_entities, entity);
            sparse_set_insert(&e->deleted_entities, entity);
            break;
    }
}
