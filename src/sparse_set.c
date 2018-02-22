#include "sparse_set.h"

void
sparse_set_init(struct sparse_set *s, struct allocator *allocator)
{
    s->allocator = allocator;
}

void
sparse_set_insert(struct sparse_set *s, u32 value)
{
    u32 dbl_cap, index, n;  //, new_size;
    u32 *tmp;

    if (value >= s->capacity)
    {
        dbl_cap = (value + 1) * 2;

        tmp = s->sparse;
        s->sparse = alloc(s->allocator, sizeof(u32) * dbl_cap);
        memcpy(s->sparse, tmp, sizeof(u32) * s->capacity);
        dealloc(s->allocator, tmp);

        tmp = s->dense;
        s->dense = alloc(s->allocator, sizeof(u32) * dbl_cap);
        memcpy(s->dense, tmp, sizeof(u32) * s->capacity);
        dealloc(s->allocator, tmp);

        // new_size = (dbl_cap - s->capacity) * sizeof(u32);

        // memset(s->sparse + s->capacity, 0, new_size);
        // memset(s->dense + s->capacity, 0, new_size);

        s->capacity = dbl_cap;
    }
    index = s->sparse[value];
    n = s->population;
    if (index >= n || s->dense[index] != value)
    {
        s->dense[n] = value;
        s->sparse[value] = n;
        s->population = n + 1;
    }
}

void
sparse_set_delete(struct sparse_set *s, u32 value)
{
    u32 temp, index, n;
    if (value >= s->capacity || s->population == 0)
        return;

    index = s->sparse[value];
    n = s->population - 1;
    if (index <= n || s->dense[index] == value)
    {
        temp = s->dense[n];
        s->dense[index] = temp;
        s->sparse[temp] = index;
        s->population = n;
    }
}

u32
sparse_set_pop(struct sparse_set *s)
{
    return s->population >= 1 ? s->dense[--s->population] : UINT_MAX;
}

void
sparse_set_clear(struct sparse_set *s)
{
    s->population = 0;
}

bool
sparse_set_is_empty(struct sparse_set *s)
{
    return s->population == 0;
}
