#include "allocators/allocator.h"

void
allocator_init(struct allocator *allocator,
               void *internal_allocator,
               void *(*alloc_func)(void *, u32),
               void (*dealloc_func)(void *, void *))
{
    allocator->internal_allocator = internal_allocator;
    allocator->alloc = alloc_func;
    allocator->dealloc = dealloc_func;
}

void *
alloc(struct allocator *allocator, u32 size)
{
    return allocator->alloc(allocator->internal_allocator, size);
}

void
dealloc(struct allocator *allocator, void *ptr)
{
    allocator->dealloc(allocator->internal_allocator, ptr);
}