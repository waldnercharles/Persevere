#ifndef NERD_MAP_H
#define NERD_MAP_H
#include "nerd_engine.h"

struct map_node
{
    u32 hash;
    char *key;
    void *value;
    struct map_node *next;
};

struct map
{
    u32 count;
    u32 capacity;
    struct map_node **buckets;
};

struct map *map_alloc(u32 capacity);
void map_free(struct map *m);

bool map_contains(struct map *m, char *key);

void *map_get(struct map *m, char *key);
void map_set(struct map *m, char *key, void *value);
void map_remove(struct map *m, char *key);

#endif