#pragma once

#include "nerd_memory.h"

// clang-format off
#define array_free(a)           ((a) ? free(array__raw(a)),0 : 0)
#define array_push(a,v)         (array_push_sz(a,v,sizeof(*(a))))
#define array_push_sz(a,v,sz)   (array__grow_if_required_sz(a,1,sz), (a)[array__len(a)++] = (v))
#define array_pop(a)            ((a)[--array__len(a)])
#define array_add(a,n)          (array_add_sz(a,n,sizeof(*(a))))
#define array_add_sz(a,n,sz)    (array__grow_if_required_sz(a,n,sz), array__len(a)+=(n), &(a)[array__len(a)-(n)])
#define array_count(a)          ((a) ? array__len(a) : 0)

#define array__raw(a)           ((uint *)(a) - 2)
#define array__len(a)           (array__raw(a)[0])
#define array__cap(a)           (array__raw(a)[1])

#define array__grow_required(a,n)           ((a)==0 || array__len(a)+(n) >= array__cap(a))
#define array__grow(a,n)                    (array__grow_sz(a,n,sizeof(*(a))))
#define array__grow_sz(a,n,sz)              ((a)=array__grow_internal((a),(n),(sz)))
#define array__grow_if_required(a,n)        (array__grow_if_required_sz(a,n,sizeof(*(a))))
#define array__grow_if_required_sz(a,n,sz)  (array__grow_required(a,n) ? array__grow_sz(a,n,sz) : 0)

#define array_for_each(a,v)  for((v)=(a); (v)<(a)+array__len(a); ++(v))
// clang-format on

static void *array__grow_internal(void *arr, int inc, size_t item_size)
{
    int double_capacity = arr ? array__cap(arr) * 2 : 0;
    int min_capacity = array_count(arr) + inc;
    int new_capacity = double_capacity > min_capacity ? double_capacity
                                                      : min_capacity;

    int *ptr = realloc(arr ? array__raw(arr) : 0, item_size * new_capacity + sizeof(int) * 2);

    if (ptr == NULL)
    {
        return NULL;
    }

    if (!arr)
    {
        ptr[0] = 0;
    }

    ptr[1] = new_capacity;
    return ptr + 2;
}
