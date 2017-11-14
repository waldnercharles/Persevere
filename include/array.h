#ifndef ARRAY_H
#define ARRAY_H
#include "std.h"

#define array_set_cap_sz(a, n, sz)                                             \
    ((a) == NULL                                                               \
         ? array__grow_sz(a, n, sz)                                            \
         : ((array__cap(a) > n) ? (a)                                          \
                                : array__grow_sz(a, n - array__cap(a), sz)))

// clang-format off
#define array_init(a)           (array__grow(a,0))
#define array_set_cap(a,n)      (array_set_cap_sz(a,n,sizeof(*(a))))
#define array_free(a)           ((a) ? free(array__raw(a)),NULL : NULL)
#define array_push(a,v)         (array_push_sz(a,v,sizeof(*(a))))
#define array_push_sz(a,v,sz)   (array__grow_if_required_sz(a,1,sz), (a)[array__len(a)++] = (v))
#define array_pop(a)            ((a)[--array__len(a)])
#define array_deleten(a,i,n)    (array__delete_len_internal((void **) &(a), sizeof(*(a)), i, n))
#define array_delete(a,i)       (array_deleten(a,i,1))
#define array_delete_fast(a,i)  ((a)[i]=array_pop(a))
#define array_addn(a,n)         (array_add_sz(a,n,sizeof(*(a))))
#define array_add_sz(a,n,sz)    (array__grow_if_required_sz(a,n,sz), array__len(a)+=(n), &(a)[array__len(a)-(n)])
#define array_count(a)          ((a) ? array__len(a) : 0)

#define array__raw(a)           ((u32 *)(a) - 2)
#define array__len(a)           (array__raw(a)[0])
#define array__cap(a)           (array__raw(a)[1])

#define array__grow_required(a,n)           ((a)==NULL || array__len(a)+(n) >= array__cap(a))
#define array__grow(a,n)                    (array__grow_sz(a,n,sizeof(*(a))))
#define array__grow_sz(a,n,sz)              ((a)=array__grow_internal((a),(n),(sz)))
#define array__grow_if_required(a,n)        (array__grow_if_required_sz(a,n,sizeof(*(a))))
#define array__grow_if_required_sz(a,n,sz)  (array__grow_required(a,n) ? array__grow_sz(a,n,sz) : 0)

#define array_for_each(v,a)  for((v)=(a); (v)<(a)+array__len(a); ++(v))
// clang-format on

void *array__grow_internal(void *arr, u32 inc, u32 item_size);
void array__delete_len_internal(void **arr_ptr, u32 size, u32 i, u32 n);
#endif