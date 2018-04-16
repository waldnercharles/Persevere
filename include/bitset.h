#ifndef BITSET_H
#define BITSET_H
#include "std.h"
#include "allocators/allocator.h"

struct bitset
{
    u8 *bytes;
    u32 capacity;

    struct allocator *allocator;
};

void bitset_init(struct bitset *bitset, struct allocator *allocator);
u32 bitset_mask(u32 b);
u32 bitset_slot(u32 b);
u32 bitset_nslots(u32 b);
void bitset_set(u8 *bytes, u32 b);
void bitset_clear(u8 *bytes, u32 b);
b32 bitset_test(u8 *bytes, u32 b);
void bitset_insert(struct bitset *bitset, u32 value);
void bitset_delete(struct bitset *bitset, u32 value);

#define bitset_for_each(i, b)                                                  \
    for ((i) = 0; (i) < (b)->capacity; (i)++)                                  \
        if (bitset_test((b)->bytes, (i)))
#endif