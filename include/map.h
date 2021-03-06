#ifndef MAP_H
#define MAP_H
#include "std.h"
#include "allocators/allocator.h"

struct map_node
{
    u32 hash;
    char *key;
    void *value;
    struct map_node *next;
};

struct map
{
    struct allocator *allocator;
    u32 count;
    u32 capacity;
    struct map_node **buckets;
};

struct map *map_alloc(struct allocator *allocator, u32 capacity);
void map_free(struct map *m);

b32 map_contains(struct map *m, char *key);

void *map_get(struct map *m, char *key);
void map_set(struct map *m, char *key, void *value);
void map_remove(struct map *m, char *key);

#endif