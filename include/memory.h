#ifndef MEMORY_H
#define MEMORY_H
#include "allocators/allocator.h"

struct memory
{
    struct allocator *permanent;
    struct allocator *transient;
};

#endif