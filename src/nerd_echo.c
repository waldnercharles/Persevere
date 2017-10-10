#include "nerd_echo.h"

static void *
echo__entity_get_data(struct echo *echo, uint entity)
{
    return (void *)((uint8 *)echo->data + (echo->data_width * entity));
}

static inline void
echo__subscribe(struct echo_system *system, uint entity)
{
    bitset_insert(&system->entities, entity);
}

static inline void
echo__unsubscribe(struct echo_system *system, uint entity)
{
    bitset_delete(&system->entities, entity);
}

static void
echo__check(struct echo *echo, struct echo_system *system, uint entity)
{
    uint *component;
    uint8 *entity_components = echo__entity_get_data(echo, entity);

    array_for_each (system->watched_components, component)
    {
        if (!bitset_test(entity_components, *component))
        {
            echo__unsubscribe(system, entity);
            return;
        }
    }
    echo__subscribe(system, entity);
}

void
echo_init(struct echo *echo)
{
    struct echo_component *component;
    uint component_bytes = bitset_nslots(echo->num_components);

    echo->data_width += component_bytes;
    array_for_each (echo->components, component)
    {
        component->offset += component_bytes;
    }

    echo->initialized = 1;
}

void
echo_process(struct echo *echo, float dt)
{
    uint *entity;
    struct echo_system *system;

    // TODO: Handle added event
    sparse_set_clear(&echo->added_entities);

    // enabled entities
    sparse_set_for_each (&echo->enabled_entities, entity)
    {
        array_for_each (echo->systems, system)
        {
            echo__check(echo, system, *entity);
        }
        // TODO: Handle enabled event
        bitset_insert(&echo->active_entities, *entity);
    }
    sparse_set_clear(&echo->enabled_entities);

    // disabled entities
    sparse_set_for_each (&echo->disabled_entities, entity)
    {
        array_for_each (echo->systems, system)
        {
            echo__unsubscribe(system, *entity);
        }
        // TODO: Handle disabled event
        bitset_delete(&echo->active_entities, *entity);
    }
    sparse_set_clear(&echo->disabled_entities);

    // deleted entities
    sparse_set_for_each (&echo->deleted_entities, entity)
    {
        array_for_each (echo->systems, system)
        {
            echo__unsubscribe(system, *entity);
        }
        // TODO: Handle deleted event
        for (uint i = 0; i < echo->num_components; i++)
        {
            echo_entity_del_component(echo, *entity, i);
        }
        sparse_set_insert(&echo->free_entities, *entity);
    }
    sparse_set_clear(&echo->deleted_entities);

    // systems
    array_for_each (echo->systems, system)
    {
        bitset_for_each (&system->entities, *entity)
        {
            system->process(echo, *entity, dt);
        }
    }
}

// component
void
echo_component_create(struct echo *echo,
                      const char *name,
                      size_t size,
                      uint *component)
{
    struct echo_component c = { 0 };
    c.name = name;
    c.size = size;
    c.offset = echo->data_width;

    echo->data_width += size;

    array_push(echo->components, c);
    *component = ++(echo->num_components) - 1;
}

// system
void
echo_system_create(struct echo *echo,
                   const char *name,
                   echo__process_func_t process,
                   uint *system)
{
    struct echo_system s = { 0 };
    s.name = name;
    s.process = process;

    array_push(echo->systems, s);
    *system = ++(echo->num_systems) - 1;
}

void
echo_system_watch_component(struct echo *echo, uint system, uint component)
{
    array_push(echo->systems[system].watched_components, component);
}

void
echo_system_process(struct echo *echo, uint system, float dt)
{
    (void)echo, (void)system, (void)dt;
    // TODO
}

// entity
void
echo_entity_create(struct echo *echo, uint *entity)
{
    uint id = sparse_set_is_empty(&echo->free_entities)
                  ? echo->next_entity_id++
                  : sparse_set_pop(&echo->free_entities);

    array__grow_if_required_sz(echo->data, id, echo->data_width);

    *entity = id;
}

void
echo_entity_clone(struct echo *echo, uint prototype, uint *entity)
{
    (void)echo, (void)prototype, (void)entity;
    // TODO
}

void
echo_entity_get_component(struct echo *echo,
                          uint entity,
                          uint component,
                          void **data)
{
    uint8 *entity_data = echo__entity_get_data(echo, entity);
    struct echo_component *component_info = &(echo->components[component]);

    // TODO: Check if component's bit is set before trying to grab it?

    *data = (void *)(entity_data + component_info->offset);
}

void
echo_entity_set_component(struct echo *echo,
                          uint entity,
                          uint component,
                          const void *data)
{
    uint8 *entity_data = echo__entity_get_data(echo, entity);
    struct echo_component *component_info = &(echo->components[component]);

    bitset_set(entity_data, component);

    memcpy((entity_data + component_info->offset), data, component_info->size);
}

void
echo_entity_del_component(struct echo *echo, uint entity, uint component)
{
    uint8 *entity_data = echo__entity_get_data(echo, entity);
    bitset_clear(entity_data, component);
}

void
echo_entity_set_state(struct echo *echo, uint entity, enum echo_state state)
{
    switch (state)
    {
        case ECHO_ENTITY_ADDED:
            sparse_set_insert(&echo->added_entities, entity);
            sparse_set_insert(&echo->enabled_entities, entity);
            sparse_set_delete(&echo->disabled_entities, entity);
            sparse_set_delete(&echo->deleted_entities, entity);
            break;
        case ECHO_ENTITY_ENABLED:
            sparse_set_insert(&echo->enabled_entities, entity);
            sparse_set_delete(&echo->disabled_entities, entity);
            sparse_set_delete(&echo->deleted_entities, entity);
            break;
        case ECHO_ENTITY_DISABLED:
            sparse_set_delete(&echo->enabled_entities, entity);
            sparse_set_insert(&echo->disabled_entities, entity);
            sparse_set_delete(&echo->deleted_entities, entity);
            break;
        case ECHO_ENTITY_DELETED:
            sparse_set_delete(&echo->added_entities, entity);
            sparse_set_delete(&echo->enabled_entities, entity);
            sparse_set_insert(&echo->disabled_entities, entity);
            sparse_set_insert(&echo->deleted_entities, entity);
            break;
    }
}
