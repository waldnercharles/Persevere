#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "log.h"

void *
array__init(struct allocator *allocator, u32 cap, u32 element_size)
{
    struct array *arr;

    arr = alloc(allocator, sizeof(struct array) + cap * element_size);
    arr->len = 0;
    arr->cap = cap;
    arr->element_size = element_size;
    arr->allocator = allocator;

    return arr + 1;
}

void *
array__grow_internal(void *ptr, u32 inc)
{
    struct array *arr, *new_arr;
    u32 min_cap, dbl_cap, new_cap;
    u32 new_size;

    if (ptr == NULL)
    {
        log_error("Array not initialized.");
    }

    arr = array__raw(ptr);

    dbl_cap = arr->cap * 2;
    min_cap = arr->len + inc;
    new_cap = dbl_cap > min_cap ? dbl_cap : min_cap;

    new_size = arr->element_size * new_cap + sizeof(struct array);

    new_arr = alloc(arr->allocator, new_size);
    memcpy(new_arr, arr, arr->element_size * arr->cap + sizeof(struct array));
    dealloc(arr->allocator, arr);

    if (new_arr == NULL)
    {
        return NULL;
    }

    new_arr->cap = new_cap;

    return new_arr + 1;
}

void
array__delete_len_internal(void *ptr, u32 idx, u32 n)
{
    struct array *arr = array__raw(ptr);

    if (n > 0)
    {
        // remove the element, shifting everything left
        memmove((u8 *)ptr + (arr->element_size * idx),
                (u8 *)ptr + (arr->element_size * (idx + n)),
                (arr->element_size * (arr->len - (idx + n))));

        arr->len -= n;
    }
}