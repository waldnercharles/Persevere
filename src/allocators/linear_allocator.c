#include "allocators/linear_allocator.h"

void
linear_allocator_init(struct linear_allocator *a, u32 size)
{
    a->start = malloc(size);
    a->end = a->start + size;

    memset(a->start, 0, size);

    a->offset = 0;
}

void
linear_allocator_free(struct linear_allocator *a)
{
    free(a->start);
}

void *
linear_allocator_alloc(void *allocator, u32 size)
{
    struct linear_allocator *a = allocator;
    u8 *addr = a->start + a->offset;

    if (addr >= a->end)
    {
        return NULL;
    }

    a->offset += size;

    return addr;
}

void
linear_allocator_dealloc(void *allocator, void *ptr)
{
    unused(allocator), unused(ptr);
    return;
}