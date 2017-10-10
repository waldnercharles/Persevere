#pragma once

#include "nerd.h"
#include "nerd_memory.h"
#include "nerd_typedefs.h"

struct bitset
{
    uint8 *bytes;
    uint capacity;
};

uint bitset_mask(uint b);
uint bitset_slot(uint b);
uint bitset_nslots(uint b);
void bitset_set(uint8 *bytes, uint b);
void bitset_clear(uint8 *bytes, uint b);
bool bitset_test(uint8 *bytes, uint b);
void bitset_insert(struct bitset *bitset, uint value);
void bitset_delete(struct bitset *bitset, uint value);

#define bitset_for_each(b, i)                                                  \
    for ((i) = 0; (i) < (b)->capacity; (i)++)                                  \
        if (bitset_test((b)->bytes, (i)))