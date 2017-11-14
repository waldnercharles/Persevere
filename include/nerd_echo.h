#ifndef NERD_ECHO_H
#define NERD_ECHO_H
#include "nerd_engine.h"
#include "nerd_bitset.h"
#include "nerd_sparse_set.h"

struct echo;

typedef void(*echo__process_func_t);

enum echo_state
{
    ECHO_STATE_ADDED,
    ECHO_STATE_ENABLED,
    ECHO_STATE_DISABLED,
    ECHO_STATE_DELETED,
};

struct echo_system
{
    const char *name;
    void (*process_begin)(struct echo *echo, void *u_data);
    void (*process)(struct echo *echo, void *u_data, u32 entity, f32 dt);
    void (*process_end)(struct echo *echo, void *u_data);
    u32 *watched_components;
    struct bitset entities;
};

struct echo_component
{
    const char *name;
    u32 size;
    u32 offset;
};

struct echo
{
    bool initialized;

    u32 data_width;
    u32 data_len;
    u32 data_cap;
    void *data;

    struct sparse_set added_entities;
    struct sparse_set enabled_entities;
    struct sparse_set deleted_entities;
    struct sparse_set disabled_entities;
    struct sparse_set dirty_entities;

    struct bitset active_entities;

    u32 next_entity_id;
    struct sparse_set free_entities;

    u32 num_components;
    struct echo_component *components;

    u32 num_systems;
    struct echo_system *systems;
};

// echo
struct echo *echo_alloc();
void echo_init(struct echo *echo);
void echo_process(struct echo *echo, void *u_data, f32 dt);

// component
void echo_create_component(struct echo *echo,
                           char *name,
                           u32 size,
                           u32 *component_ptr);

// system
void echo_create_system(struct echo *echo,
                        char *name,
                        void (*process_begin)(struct echo *, void *),
                        void (*process)(struct echo *, void *, u32, f32),
                        void (*process_end)(struct echo *, void *),
                        u32 *system_ptr);

void echo_watch(struct echo *e, u32 system, u32 component);
void echo_process_system(struct echo *echo, u32 system, void *u_data, f32 dt);

// entity
void echo_create_entity(struct echo *echo, u32 *entity_ptr);
void echo_clone_entity(struct echo *echo, u32 entity, u32 *entity_ptr);
void echo_get_component(struct echo *echo,
                        u32 entity,
                        u32 component,
                        void **component_data_ptr);

void echo_set_component(struct echo *echo,
                        u32 entity,
                        u32 component,
                        void *component_data);

void echo_rem_component(struct echo *echo, u32 entity, u32 component);
void echo_set_state(struct echo *echo, u32 entity, u32 state);
#endif