#pragma once

#include "nerd.h"
#include "nerd_memory.h"
#include "nerd_typedefs.h"

struct sparse_set
{
    uint *sparse;
    uint *dense;
    uint population;
    uint capacity;
};

#define sparse_set_for_each(s, v)                                              \
    for ((v) = (s)->dense; (v) < (s)->dense + (s)->population; ++(v))

void sparse_set_insert(struct sparse_set *s, uint value);
void sparse_set_delete(struct sparse_set *s, uint value);
uint sparse_set_pop(struct sparse_set *s);
void sparse_set_clear(struct sparse_set *s);
int sparse_set_is_empty(struct sparse_set *s);