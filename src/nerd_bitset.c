#include "nerd_bitset.h"

// clang-format off
uint bitset_mask(uint b) { return (1 << (b & 7)); }
uint bitset_slot(uint b) { return b >> 3; }
uint bitset_nslots(uint b) { return (b + 7) >> 3; }
void bitset_set(uint8 *bytes, uint b) { bytes[bitset_slot(b)] |= bitset_mask(b); }
void bitset_clear(uint8 *bytes, uint b) { bytes[bitset_slot(b)] &= ~bitset_mask(b); }
bool bitset_test(uint8 *bytes, uint b) { return bytes[bitset_slot(b)] & bitset_mask(b); }
// clang-format on

void
bitset_insert(struct bitset *bitset, uint value)
{
    uint old_size, new_size, new_capacity;
    if (value >= bitset->capacity)
    {
        new_capacity = (value + 1) * 2;
        old_size = bitset_nslots(bitset->capacity);
        new_size = bitset_nslots(new_capacity);

        bitset->bytes = realloc(bitset->bytes, new_size);
        memset(bitset->bytes + old_size, 0, new_size - old_size);

        bitset->capacity = new_capacity;
    }

    bitset_set(bitset->bytes, value);
}

void
bitset_delete(struct bitset *bitset, uint value)
{
    if (value < bitset->capacity)
    {
        bitset_clear(bitset->bytes, value);
    }
}