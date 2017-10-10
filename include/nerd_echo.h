#pragma once

#include "nerd.h"
#include "nerd_array.h"
#include "nerd_bitset.h"
#include "nerd_sparse_set.h"
#include "nerd_typedefs.h"

struct echo;

typedef void (*echo__process_func_t)(struct echo *echo, uint entity, float dt);

enum echo_state
{
    ECHO_ENTITY_ADDED,
    ECHO_ENTITY_ENABLED,
    ECHO_ENTITY_DISABLED,
    ECHO_ENTITY_DELETED,
};

struct echo_system
{
    const char *name;
    echo__process_func_t process;
    uint *watched_components;
    struct bitset entities;
};

struct echo_component
{
    const char *name;
    size_t size;
    size_t offset;
};

struct echo
{
    int initialized;

    size_t data_width;
    size_t data_len;
    size_t data_cap;
    void *data;

    struct sparse_set added_entities;
    struct sparse_set enabled_entities;
    struct sparse_set deleted_entities;
    struct sparse_set disabled_entities;
    struct sparse_set dirty_entities;

    struct bitset active_entities;

    uint next_entity_id;
    struct sparse_set free_entities;

    size_t num_components;
    struct echo_component *components;

    size_t num_systems;
    struct echo_system *systems;
};

void echo_init(struct echo *echo);

void echo_process(struct echo *echo, float dt);

void echo_component_create(struct echo *echo,
                           const char *name,
                           size_t size,
                           uint *component);

void echo_system_create(struct echo *echo,
                        const char *name,
                        echo__process_func_t process,
                        uint *system);

void echo_system_watch_component(struct echo *echo,
                                 uint system,
                                 uint component);

void echo_system_process(struct echo *echo, uint system, float dt);

void echo_entity_create(struct echo *echo, uint *entity);

void echo_entity_clone(struct echo *echo, uint prototype, uint *entity);

void echo_entity_get_component(struct echo *echo,
                               uint entity,
                               uint component,
                               void **data);

void echo_entity_set_component(struct echo *echo,
                               uint entity,
                               uint component,
                               const void *data);

void echo_entity_del_component(struct echo *echo, uint entity, uint component);

void echo_entity_set_state(struct echo *echo, uint entity, uint state);
