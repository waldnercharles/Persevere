#include "array.h"
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "vec.h"
#include "std.h"
#include "allocators/allocator.h"

void *
array_get(struct array *a, u32 idx)
{
    return ((u8 *)a->data) + (idx * a->element_size);
}

void
array_set(struct array *a, u32 idx, const void *val)
{
    memcpy(array_get(a, idx), val, a->element_size);
}

void
array_set_many(struct array *a, u32 idx, const void *val, u32 n)
{
    memcpy(array_get(a, idx), val, a->element_size * n);
}

void
array_insert(struct array *a, u32 idx, const void *val)
{
    if (a->cap < a->len + 1)
    {
        array_grow(a);
    }

    memmove(array_get(a, idx + 1),
            array_get(a, idx),
            a->element_size * (a->len - idx));

    memcpy(array_get(a, idx), val, a->element_size);

    a->len++;
}

void
array_remove(struct array *a, u32 idx)
{
    memmove(array_get(a, idx),
            array_get(a, idx + 1),
            a->element_size * (a->len - idx));

    a->len--;
}

void
array_remove_range(struct array *a, u32 first_idx, u32 last_idx)
{
    memmove(array_get(a, first_idx),
            array_get(a, last_idx),
            a->element_size * (a->len - last_idx));

    a->len -= last_idx - first_idx;
}

void
array_append(struct array *a, const void *vals, u32 n)
{
    u32 new_len = a->len + n;
    if (a->cap < new_len)
    {
        array_grow_to_at_least(a, new_len);
    }

    memcpy(array_end(a), vals, a->element_size * n);
    a->len = new_len;
}

void
array_push(struct array *a, const void *val)
{
    array_append(a, val, 1);
}

void *
array_pop(struct array *a)
{
    if (a->len > 0)
    {
        return array_get(a, --a->len);
    }
    else
    {
        return NULL;
    }
}

void *
array_first(struct array *a)
{
    return a->data;
}

void *
array_last(struct array *a)
{
    return array_empty(a) ? NULL : array_get(a, a->len - 1);
}

void *
array_begin(struct array *a)
{
    return a->data;
}

void *
array_next(struct array *a, void *i)
{
    return ((u8 *)i) + a->element_size;
}

void *
array_end(struct array *a)
{
    return ((u8 *)a->data) + (a->len * a->element_size);
}

b32
array_empty(struct array *a)
{
    return a->len == 0;
}

u32
array_size(struct array *a)
{
    return a->cap * a->element_size;
}

void
array_alloc(struct allocator *allocator,
            u32 cap,
            u32 element_size,
            struct array **a)
{
    (*a) = alloc(allocator, sizeof(struct array));
    (*a)->len = 0;
    (*a)->cap = u32_max(cap, 1);
    (*a)->element_size = element_size;
    (*a)->allocator = allocator;
    (*a)->data = alloc(allocator, array_size(*a));
}

void
array_free(struct array *a)
{
    dealloc(a->allocator, a);
}

void
array_grow_to(struct array *a, u32 len)
{
    u32 new_size;
    void *new_data;

    new_size = len * a->element_size;

    new_data = alloc(a->allocator, new_size);
    memcpy(new_data, a->data, array_size(a));
    dealloc(a->allocator, a->data);

    a->data = new_data;

    a->cap = len;
}

void
array_grow_to_at_least(struct array *a, u32 len)
{
    array_grow_to(a, u32_max(len, a->cap * 2));
}

void
array_grow_by(struct array *a, u32 n)
{
    array_grow_to(a, a->cap + n);
}

void
array_grow_by_at_least(struct array *a, u32 n)
{
    array_grow_to_at_least(a, a->cap + n);
}

void
array_grow(struct array *a)
{
    array_grow_to(a, a->cap * 2);
}

void
array_clear(struct array *a)
{
    a->len = 0;
    memset(a->data, 0, array_size(a));
}