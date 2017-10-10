#include "nerd_sparse_set.h"

void
sparse_set_insert(struct sparse_set *s, uint value)
{
    uint dbl_cap, index, n;
    if (value >= s->capacity)
    {
        dbl_cap = (value + 1) * 2;
        s->sparse = realloc(s->sparse, sizeof(uint) * dbl_cap);
        s->dense = realloc(s->dense, sizeof(uint) * dbl_cap);

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
sparse_set_delete(struct sparse_set *s, uint value)
{
    uint temp, index, n;
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

uint
sparse_set_pop(struct sparse_set *s)
{
    return s->population >= 1 ? s->dense[--s->population] : UINT_MAX;
}

void
sparse_set_clear(struct sparse_set *s)
{
    s->population = 0;
}

int
sparse_set_is_empty(struct sparse_set *s)
{
    return s->population == 0;
}
