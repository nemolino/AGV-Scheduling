#include "solver.h"

#include <string.h>

#define BITSET_INDEX_OF(x) ((x) / 64)
#define BITSET_OFFSET_OF(x) ((x) % 64)

BitSet bitset_create() 
{
    BitSet bs = (uint64_t*)safe_calloc(BITSET_ARR_LEN, sizeof(uint64_t));
    return bs;
}

void bitset_destroy(BitSet bs) 
{
    free(bs);
}

void bitset_set_bit(BitSet bs, int x) 
{
    bs[BITSET_INDEX_OF(x)] |= (1ULL << BITSET_OFFSET_OF(x));
}

void bitset_clear_bit(BitSet bs, int x) 
{
    bs[BITSET_INDEX_OF(x)] &= ~(1ULL << BITSET_OFFSET_OF(x));
}

void bitset_clear_all(BitSet bs) 
{
    memset(bs, 0ULL, BITSET_ARR_LEN * sizeof(uint64_t));
}

void bitset_copy(BitSet dest, BitSet src) 
{
    for (size_t i = 0; i < BITSET_ARR_LEN; i++){
        dest[i] = src[i];
    }
}

void bitset_inplace_intersection(BitSet bs, BitSet other) 
{
    for (size_t i = 0; i < BITSET_ARR_LEN; i++) {
        bs[i] &= other[i];
    }
}

void bitset_inplace_difference(BitSet bs, BitSet other) 
{
    for (size_t i = 0; i < BITSET_ARR_LEN; i++) {
        bs[i] &= ~other[i];
    }
}

bool bitset_any(BitSet bs) 
{
    for (size_t i = 0; i < BITSET_ARR_LEN; i++) {
        if (bs[i] > 0){
            return true;
        } 
    }
    return false;
}

bool bitset_none(BitSet bs) 
{
    return !bitset_any(bs);
}

/*

// Find the next set bit starting from 'start_idx'
int bitset_next_set(BitSet* bs, int start_idx) {
    size_t start_word = BITSET_INDEX_OF(start_idx);
    int offset = BITSET_OFFSET_OF(start_idx);

    for (size_t i = start_word; i < bs.arr_size; i++) {
        uint64_t word = bs.arr[i];
        if (i == start_word) {
            word &= (~0ULL << offset); // Mask out lower bits
        }
        if (word) {
            return i * 64 + __builtin_ctzll(word); // Find the first set bit
        }
    }
    return -1; // No set bit found
}
*/