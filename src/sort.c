#include "sort.h"

void
swap(u8 *a, u8 *b, u32 n)
{
    u8 tmp;
    while (n--)
    {
        tmp = a[n];
        a[n] = b[n];
        b[n] = tmp;
    }
}

static void
satisfy_heap(u8 *a,
             s32 i,
             s32 heap_size,
             s32 size,
             void *u_data,
             s32 (*cmp)(const void *, const void *, void *))
{
    s32 l, r, lrg;
    l = i * 2;
    r = i * 2 + 1;
    if (l <= heap_size && cmp(&a[size * l], &a[size * i], u_data) > 0)
    {
        lrg = l;
    }
    else
    {
        lrg = i;
    }
    if (r <= heap_size && cmp(a + (size * r), &a[size * lrg], u_data) > 0)
    {
        lrg = r;
    }
    if (lrg != i)
    {
        swap(&a[i * size], &a[lrg * size], size);
        satisfy_heap(a, lrg, heap_size, size, u_data, cmp);
    }
}

static void
build_heap(u8 *a,
           s32 n,
           s32 size,
           void *u_data,
           s32 (*cmp)(const void *, const void *, void *))
{
    s32 heap_size, i;
    heap_size = n - 1;
    for (i = (n / 2); i >= 0; --i)
    {
        satisfy_heap(a, i, heap_size, size, u_data, cmp);
    }
}

void
heap_sort(void *a,
          s32 n,
          s32 size,
          void *u_data,
          s32 (*cmp)(const void *, const void *, void *))
{
    s32 heap_size, i;

    build_heap(a, n, size, u_data, cmp);
    heap_size = n - 1;
    for (i = heap_size; i >= 0; --i)
    {
        swap(a, (u8 *)a + (heap_size * size), size);
        --heap_size;
        satisfy_heap(a, 0, heap_size, size, u_data, cmp);
    }
}