#pragma once

#include <limits.h>

#include "nerd_memory.h"
#include "nerd_typedefs.h"

struct sparse_set
{
    uint *sparse;
    uint *dense;
    uint population;
    uint capacity;
};

#define sparse_set_for_each(s,v)  for((v)=(s)->dense;(v)<(s)->dense+(s)->population;++(v))


void sparse_set_insert(struct sparse_set *s, uint value)
{
    if (value >= s->capacity)
    {
	uint dbl_cap = (value + 1) * 2;
	s->sparse = realloc(s->sparse, sizeof(uint) * dbl_cap);
	s->dense = realloc(s->dense, sizeof(uint) * dbl_cap);

	s->capacity = dbl_cap;
    }
    uint index = s->sparse[value];
    uint n = s->population;
    if (index >= n || s->dense[index] != value)
    {
	s->dense[n] = value;
	s->sparse[value] = n;
	s->population = n + 1;
    }
}


void sparse_set_delete(struct sparse_set *s, uint value)
{
    if (value >= s->capacity || s->population == 0) return;

    uint index = s->sparse[value];
    uint n = s->population - 1;
    if (index <= n || s->dense[index] == value)
    {
	uint temp = s->dense[n];
	s->dense[index] = temp;
	s->sparse[temp] = index; 
	s->population = n;
    }
}


uint sparse_set_pop(struct sparse_set *s)
{
    return s->population >= 1 ? s->dense[--s->population] : UINT_MAX;
}


void sparse_set_clear(struct sparse_set *s)
{
    s->population = 0;
}


int sparse_set_is_empty(struct sparse_set *s)
{
    return s->population == 0;
}
