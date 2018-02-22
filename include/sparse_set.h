#ifndef SPARSE_SET_H
#define SPARSE_SET_H
#include "std.h"
#include "allocators/allocator.h"

struct sparse_set
{
    u32 *sparse;
    u32 *dense;
    u32 population;
    u32 capacity;

    struct allocator *allocator;
};

#define sparse_set_for_each(v, i, s)                                           \
    for ((i) = 0, (v) = (s)->dense ? (s)->dense[i] : 0; (i) < (s)->population; \
         (i)++, (v) = (s)->dense[i])

void sparse_set_init(struct sparse_set *set, struct allocator *allocator);

void sparse_set_insert(struct sparse_set *s, u32 value);
void sparse_set_delete(struct sparse_set *s, u32 value);
u32 sparse_set_pop(struct sparse_set *s);
void sparse_set_clear(struct sparse_set *s);
bool sparse_set_is_empty(struct sparse_set *s);
#endif