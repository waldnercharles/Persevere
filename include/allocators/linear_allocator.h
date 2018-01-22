#ifndef LINEAR_ALLOCATOR_H
#define LINEAR_ALLOCATOR_H

#include "std.h"

struct linear_allocator
{
    u8 *start, *end;
    u32 offset;
};

void linear_allocator_init(struct linear_allocator *a, u32 size);
void linear_allocator_free(struct linear_allocator *a);

void *linear_allocator_alloc(void *allocator, u32 size);
void linear_allocator_dealloc(void *allocator, void *ptr);

#endif
