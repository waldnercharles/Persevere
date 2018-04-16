#ifndef ARRAY_H
#define ARRAY_H
#include "std.h"
#include "allocators/allocator.h"

struct array
{
    u32 len;
    u32 cap;
    u32 element_size;
    struct allocator *allocator;
    u8 *data;
};

void *array_get(struct array *a, u32 idx);
void array_set(struct array *a, u32 idx, const void *val);
void array_set_many(struct array *a, u32 idx, const void *val, u32 n);
void array_insert(struct array *a, u32 idx, const void *val);
void array_remove(struct array *a, u32 idx);
void array_remove_range(struct array *a, u32 first_idx, u32 last_idx);
void array_append(struct array *a, const void *vals, u32 n);
void array_push(struct array *a, const void *val);
void *array_pop(struct array *a);
void *array_first(struct array *a);
void *array_last(struct array *a);
void *array_begin(struct array *a);
void *array_next(struct array *a, void *i);
void *array_end(struct array *a);
b32 array_empty(struct array *a);
u32 array_size(struct array *a);
void array_clear(struct array *a);

void array_alloc(struct allocator *allocator,
                 u32 cap,
                 u32 element_size,
                 struct array **a);

void array_free(struct array *a);

void array_grow(struct array *a);
void array_grow_to(struct array *a, u32 len);
void array_grow_to_at_least(struct array *a, u32 len);
void array_grow_by(struct array *a, u32 n);
void array_grow_by_at_least(struct array *a, u32 n);

#define array_for_each(i, a)                                                   \
    for ((i) = array_begin(a); (i) != array_end(a); (i) = array_next(a, i))

#endif