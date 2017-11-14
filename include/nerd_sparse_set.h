#ifndef NERD_SPARSE_SET_H
#define NERD_SPARSE_SET_H
#include "nerd_engine.h"

struct sparse_set
{
    u32 *sparse;
    u32 *dense;
    u32 population;
    u32 capacity;
};

#define sparse_set_for_each(v, i, s)                                           \
    for ((i) = 0, (v) = (s)->dense ? (s)->dense[i] : 0; (i) < (s)->population; \
         (i)++, (v) = (s)->dense[i])

void sparse_set_insert(struct sparse_set *s, u32 value);
void sparse_set_delete(struct sparse_set *s, u32 value);
u32 sparse_set_pop(struct sparse_set *s);
void sparse_set_clear(struct sparse_set *s);
bool sparse_set_is_empty(struct sparse_set *s);
#endif