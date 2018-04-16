#include <stdlib.h>
#include <string.h>

#include "bitset.h"

// clang-format off
u32 bitset_mask(u32 b) { return (1 << (b & 7)); }
u32 bitset_slot(u32 b) { return b >> 3; }
u32 bitset_nslots(u32 b) { return (b + 7) >> 3; }
void bitset_set(u8 *bytes, u32 b) { bytes[bitset_slot(b)] |= bitset_mask(b); }
void bitset_clear(u8 *bytes, u32 b) { bytes[bitset_slot(b)] &= ~bitset_mask(b); }
b32 bitset_test(u8 *bytes, u32 b) { return bytes[bitset_slot(b)] & bitset_mask(b); }
// clang-format on

void
bitset_init(struct bitset *bitset, struct allocator *allocator)
{
    bitset->allocator = allocator;
}

void
bitset_insert(struct bitset *bitset, u32 value)
{
    u32 old_size, new_size, new_capacity;
    u8 *tmp;

    if (value >= bitset->capacity)
    {
        new_capacity = (value + 1) * 2;
        old_size = bitset_nslots(bitset->capacity);
        new_size = bitset_nslots(new_capacity);

        tmp = bitset->bytes;
        bitset->bytes = alloc(bitset->allocator, new_size);
        memcpy(bitset->bytes, tmp, sizeof(u8) * old_size);
        dealloc(bitset->allocator, tmp);

        // bitset->bytes = realloc(bitset->bytes, new_size);
        memset(bitset->bytes + old_size, 0, new_size - old_size);

        bitset->capacity = new_capacity;
    }

    bitset_set(bitset->bytes, value);
}

void
bitset_delete(struct bitset *bitset, u32 value)
{
    if (value < bitset->capacity)
    {
        bitset_clear(bitset->bytes, value);
    }
}