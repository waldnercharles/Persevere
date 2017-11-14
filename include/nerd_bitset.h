#ifndef NERD_BITSET_H
#define NERD_BITSET_H
#include "nerd_engine.h"

struct bitset
{
    u8 *bytes;
    u32 capacity;
};

u32 bitset_mask(u32 b);
u32 bitset_slot(u32 b);
u32 bitset_nslots(u32 b);
void bitset_set(u8 *bytes, u32 b);
void bitset_clear(u8 *bytes, u32 b);
bool bitset_test(u8 *bytes, u32 b);
void bitset_insert(struct bitset *bitset, u32 value);
void bitset_delete(struct bitset *bitset, u32 value);

#define bitset_for_each(i, b)                                                  \
    for ((i) = 0; (i) < (b)->capacity; (i)++)                                  \
        if (bitset_test((b)->bytes, (i)))
#endif