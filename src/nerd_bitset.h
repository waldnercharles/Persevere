#pragma once

#include "nerd.h"
#include "nerd_memory.h"
#include "nerd_typedefs.h"

struct bitset
{
    uint8 *bytes;
    uint capacity;
};

uint bitset_mask(uint b) { return (1 << (b & 7)); }
uint bitset_slot(uint b) { return b >> 3; }
uint bitset_nslots(uint b) { return (b + 7) >> 3; }
void bitset_set(uint8 *bytes, uint b) { bytes[bitset_slot(b)] |= bitset_mask(b); }
void bitset_clear(uint8 *bytes, uint b) { bytes[bitset_slot(b)] &= ~bitset_mask(b); }
bool bitset_test(uint8 *bytes, uint b) { return bytes[bitset_slot(b)] & bitset_mask(b); }

#define bitset_for_each(b, i)                 \
    for ((i) = 0; (i) < (b)->capacity; (i)++) \
        if (bitset_test((b)->bytes, (i)))

void bitset_insert(struct bitset *bitset, uint value)
{
    if (value >= bitset->capacity)
    {
        uint new_capacity = (value + 1) * 2;
        uint old_size = bitset_nslots(bitset->capacity);
        uint new_size = bitset_nslots(new_capacity);

        bitset->bytes = realloc(bitset->bytes, new_size);
        memset(bitset->bytes + old_size, 0, new_size - old_size);

        bitset->capacity = new_capacity;
    }

    bitset_set(bitset->bytes, value);
}

void bitset_delete(struct bitset *bitset, uint value)
{
    if (value < bitset->capacity)
    {
        bitset_clear(bitset->bytes, value);
    }
}
