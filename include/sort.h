#ifndef SORT_H
#define SORT_H
#include "std.h"

void swap(u8 *a, u8 *b, u32 n);
void heap_sort(void *a,
               s32 n,
               s32 size,
               void *u_data,
               s32 (*cmp)(const void *, const void *, void *));

#endif