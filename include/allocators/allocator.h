#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include "std.h"

struct allocator
{
    void *internal_allocator;
    void *(*alloc)(void *, u32);
    void (*dealloc)(void *, void *);
};

void allocator_init(struct allocator *allocator,
                    void *internal_allocator,
                    void *(*alloc_func)(void *, u32),
                    void (*dealloc_func)(void *, void *));

void *alloc(struct allocator *allocator, u32 size);
void dealloc(struct allocator *allocator, void *ptr);

#endif