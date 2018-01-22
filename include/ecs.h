#ifndef ECS_H
#define ECS_H

#include "std.h"
#include "bitset.h"
#include "sparse_set.h"

#include "allocators/allocator.h"

struct ecs;

typedef void(*ecs__process_func_t);

enum ecs_state
{
    ECS_STATE_ADDED,
    ECS_STATE_ENABLED,
    ECS_STATE_DISABLED,
    ECS_STATE_DELETED,
};

struct ecs_system
{
    const char *name;
    void (*process_begin)(struct ecs *ecs, void *u_data);
    void (*process)(struct ecs *ecs, void *u_data, u32 entity, r32 dt);
    void (*process_end)(struct ecs *ecs, void *u_data);
    u32 *watched_components;
    struct bitset entities;
};

struct ecs_component
{
    const char *name;
    u32 size;
    u32 offset;
};

struct ecs
{
    bool initialized;
    struct allocator *allocator;

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
    struct ecs_component *components;

    u32 num_systems;
    struct ecs_system *systems;
};

// ecs
void ecs_init(struct ecs *ecs);
void ecs_process(struct ecs *ecs, void *u_data, r32 dt);

// component
void ecs_create_component(struct ecs *ecs,
                          char *name,
                          u32 size,
                          u32 *component_ptr);

// system
void ecs_create_system(struct ecs *ecs,
                       char *name,
                       void (*process_begin)(struct ecs *, void *),
                       void (*process)(struct ecs *, void *, u32, r32),
                       void (*process_end)(struct ecs *, void *),
                       u32 *system_ptr);

void ecs_watch(struct ecs *e, u32 system, u32 component);
void ecs_process_system(struct ecs *ecs, u32 system, void *u_data, r32 dt);

// entity
void ecs_create_entity(struct ecs *ecs, u32 *entity_ptr);
void ecs_clone_entity(struct ecs *ecs, u32 entity, u32 *entity_ptr);
void ecs_get_component(struct ecs *ecs,
                       u32 entity,
                       u32 component,
                       void **component_data_ptr);

void ecs_set_component(struct ecs *ecs,
                       u32 entity,
                       u32 component,
                       void *component_data);

void ecs_rem_component(struct ecs *ecs, u32 entity, u32 component);
void ecs_set_state(struct ecs *ecs, u32 entity, u32 state);
#endif