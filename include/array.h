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
};

#define array_set_cap(a, n)                                                    \
    ((a) == NULL                                                               \
         ? array__grow(a, n)                                                   \
         : ((array__cap(a) > n) ? (a) : array__grow(a, n - array__cap(a))))

// clang-format off
#define array_init(a,alloc)     ((a)=array__init(alloc,0,sizeof(*(a))))
// #define array_initn(a,alloc,n)  ((a)=array__init(alloc,n,sizeof(*(a))))
#define array_free(a)           (dealloc(array__raw(a)->allocator, array__raw(a)),NULL)
#define array_push(a,v)         (array__grow_if_required(a,1), (a)[array__len(a)++] = (v))
#define array_pop(a)            ((a)[--array__len(a)])
#define array_deleten(a,i,n)    (array__delete_len_internal(a,i,n))
#define array_delete(a,i)       (array__delete_len_internal(a,i,1))
#define array_delete_fast(a,i)  ((a)[i]=array_pop(a))
#define array_add(a,n,sz)       (array__grow_if_required(a,n), array__len(a)+=(n), &(a)[array__len(a)-(n)])
#define array_count(a)          (array__len(a))

#define array__raw(a)           ((struct array *)(a) - 1)
#define array__len(a)           (array__raw(a)->len)
#define array__cap(a)           (array__raw(a)->cap)
#define array__element_size(a)  (array__raw(a)->element_size)

#define array__grow_required(a,n)     ((a)==NULL || array__len(a)+(n) >= array__cap(a))
#define array__grow(a,n)              ((a)=array__grow_internal((a),(n)))
#define array__grow_if_required(a,n)  (array__grow_required(a,n) ? array__grow(a,n) : 0)
#define array__grow_to(a,cap)         (cap > array__cap(a) ? array__grow(a,cap-array__cap(a)) : 0)

#define array_for_each(v,a)  for((v)=(a); (v)<(a)+array__len(a); ++(v))
// clang-format on

void *array__init(struct allocator *allocator, u32 cap, u32 element_size);
void *array__grow_internal(void *arr, u32 inc);
void array__delete_len_internal(void *arr, u32 idx, u32 n);
#endif