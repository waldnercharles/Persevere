#include <stdlib.h>
#include <string.h>

#include "array.h"

void *
array__grow_internal(void *arr, u32 inc, u32 item_size)
{
    u32 double_cap = arr ? array__cap(arr) * 2 : 0;
    u32 min_cap = array_count(arr) + inc;
    u32 new_cap = double_cap > min_cap ? double_cap : min_cap;

    u32 new_size = item_size * new_cap + sizeof(u32) * 2;

    u32 *ptr = realloc(arr ? array__raw(arr) : 0, new_size);

    if (ptr == NULL)
    {
        return NULL;
    }

    if (!arr)
    {
        ptr[0] = 0;
    }

    ptr[1] = new_cap;
    return ptr + 2;
}

void
array__delete_len_internal(void **arr_ptr, u32 size, u32 i, u32 n)
{
    void *arr = *arr_ptr;
    if (n > 0)
    {
        // remove the element, shifting everything left
        memmove((u8 *)arr + (size * i),
                (u8 *)arr + (size * (i + n)),
                (size * (array_count(arr) - (i + n))));

        array__len(arr) -= n;
    }
    *arr_ptr = arr;
}