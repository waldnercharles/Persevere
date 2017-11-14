#include "nerd_echo.h"
#include "nerd_array.h"

static void *
echo__entity_get_data(struct echo *echo, u32 entity)
{
    return (void *)((u8 *)echo->data + (echo->data_width * entity));
}

static inline void
echo__subscribe(struct echo_system *system, u32 entity)
{
    bitset_insert(&system->entities, entity);
}

static inline void
echo__unsubscribe(struct echo_system *system, u32 entity)
{
    bitset_delete(&system->entities, entity);
}

static void
echo__check(struct echo *echo, struct echo_system *system, u32 entity)
{
    u32 *component;
    u8 *entity_components = echo__entity_get_data(echo, entity);

    array_for_each (component, system->watched_components)
    {
        if (!bitset_test(entity_components, *component))
        {
            echo__unsubscribe(system, entity);
            return;
        }
    }
    echo__subscribe(system, entity);
}

struct echo *
echo_alloc()
{
    struct echo *echo = malloc(sizeof(struct echo));
    memset(echo, 0, sizeof(struct echo));
    return echo;
}

void
echo_init(struct echo *echo)
{
    struct echo_component *component;
    u32 component_bytes = bitset_nslots(echo->num_components);

    echo->data_width += component_bytes;
    array_for_each (component, echo->components)
    {
        component->offset += component_bytes;
    }

    echo->initialized = 1;
}

void
echo_process(struct echo *echo, void *u_data, f32 dt)
{
    u32 entity, i, j;
    struct echo_system *system;

    // TODO: Handle added event
    sparse_set_clear(&echo->added_entities);

    // enabled entities
    sparse_set_for_each (entity, i, &echo->enabled_entities)
    {
        array_for_each (system, echo->systems)
        {
            echo__check(echo, system, entity);
        }
        // TODO: Handle enabled event
        bitset_insert(&echo->active_entities, entity);
    }
    sparse_set_clear(&echo->enabled_entities);

    // disabled entities
    sparse_set_for_each (entity, i, &echo->disabled_entities)
    {
        array_for_each (system, echo->systems)
        {
            echo__unsubscribe(system, entity);
        }
        // TODO: Handle disabled event
        bitset_delete(&echo->active_entities, entity);
    }
    sparse_set_clear(&echo->disabled_entities);

    // deleted entities
    sparse_set_for_each (entity, i, &echo->deleted_entities)
    {
        array_for_each (system, echo->systems)
        {
            echo__unsubscribe(system, entity);
        }
        // TODO: Handle deleted event
        for (j = 0; j < echo->num_components; ++j)
        {
            echo_rem_component(echo, entity, j);
        }
        sparse_set_insert(&echo->free_entities, entity);
    }
    sparse_set_clear(&echo->deleted_entities);

    // systems
    array_for_each (system, echo->systems)
    {
        if (system->process_begin)
        {
            system->process_begin(echo, u_data);
        }

        bitset_for_each (i, &(system->entities))
        {
            system->process(echo, u_data, entity, dt);
        }

        if (system->process_end)
        {
            system->process_end(echo, u_data);
        }
    }
}

// component
void
echo_create_component(struct echo *e, char *name, u32 size, u32 *component_ptr)
{
    struct echo_component c;
    c.name = name;
    c.size = size;
    c.offset = e->data_width;

    e->data_width += size;

    array_push(e->components, c);
    *component_ptr = ++(e->num_components) - 1;
}

// system
void
echo_create_system(struct echo *echo,
                   char *name,
                   void (*process_begin)(struct echo *, void *),
                   void (*process)(struct echo *, void *, u32, f32),
                   void (*process_end)(struct echo *, void *),
                   u32 *system_ptr)
{
    struct echo_system s;

    s.entities.bytes = NULL;
    s.entities.capacity = 0;

    s.name = name;
    s.process = process;
    s.process_begin = process_begin;
    s.process_end = process_end;

    s.watched_components = NULL;

    array_push(echo->systems, s);
    *system_ptr = ++(echo->num_systems) - 1;
}

void
echo_watch(struct echo *echo, u32 system, u32 component)
{
    array_push(echo->systems[system].watched_components, component);
}

void
echo_process_system(struct echo *echo, u32 system, void *u_data, f32 dt)
{
    struct echo_system *s = &echo->systems[system];
    u32 entity;

    bitset_for_each (entity, &s->entities)
    {
        s->process(echo, u_data, entity, dt);
    }
}

// entity
void
echo_create_entity(struct echo *echo, u32 *entity_ptr)
{
    u32 id = sparse_set_is_empty(&echo->free_entities)
                 ? echo->next_entity_id++
                 : sparse_set_pop(&echo->free_entities);

    array_set_cap_sz(echo->data, echo->next_entity_id, echo->data_width);

    *entity_ptr = id;
}

void
echo_clone_entity(struct echo *echo, u32 entity, u32 *entity_ptr)
{
    (void)echo, (void)entity, (void)entity_ptr;
    // TODO
}
void
echo_get_component(struct echo *e,
                   u32 entity,
                   u32 component,
                   void **component_data_ptr)
{
    u8 *entity_data = echo__entity_get_data(e, entity);
    struct echo_component *component_info = &(e->components[component]);

    // Component not found
    if (!bitset_test(entity_data, component))
    {
        return;
    }

    *component_data_ptr = (void *)(entity_data + component_info->offset);
}

void
echo_set_component(struct echo *e,
                   u32 entity,
                   u32 component,
                   void *component_data)
{
    u8 *entity_data = echo__entity_get_data(e, entity);
    struct echo_component *component_info = &(e->components[component]);

    bitset_set(entity_data, component);

    memcpy((entity_data + component_info->offset),
           component_data,
           component_info->size);
}

void
echo_rem_component(struct echo *e, u32 entity, u32 component)
{
    u8 *entity_data = echo__entity_get_data(e, entity);
    bitset_clear(entity_data, component);
}

void
echo_set_state(struct echo *e, u32 entity, enum echo_state state)
{
    switch (state)
    {
        case ECHO_STATE_ADDED:
            sparse_set_insert(&e->added_entities, entity);
            sparse_set_insert(&e->enabled_entities, entity);
            sparse_set_delete(&e->disabled_entities, entity);
            sparse_set_delete(&e->deleted_entities, entity);
            break;
        case ECHO_STATE_ENABLED:
            sparse_set_insert(&e->enabled_entities, entity);
            sparse_set_delete(&e->disabled_entities, entity);
            sparse_set_delete(&e->deleted_entities, entity);
            break;
        case ECHO_STATE_DISABLED:
            sparse_set_delete(&e->enabled_entities, entity);
            sparse_set_insert(&e->disabled_entities, entity);
            sparse_set_delete(&e->deleted_entities, entity);
            break;
        case ECHO_STATE_DELETED:
            sparse_set_delete(&e->added_entities, entity);
            sparse_set_delete(&e->enabled_entities, entity);
            sparse_set_insert(&e->disabled_entities, entity);
            sparse_set_insert(&e->deleted_entities, entity);
            break;
    }
}
