#pragma once
#include "stream_algorithms_wave_a.h"
#include <cstddef>

// Native owners for the embedded inflater. Retail offsets belong to the x86
// image only; allocator callbacks return storage aligned for these native types.
struct D2ClientInflateAllocator {
    void* opaque;
    void* (*allocate)(void*, std::size_t count, std::size_t size);
    void (*release)(void*, void*);
};
struct D2ClientInflateHuft {
    uint8_t operation, bits;
    union { uint32_t base; D2ClientInflateHuft* next; } value;
};
struct D2ClientInflateCodes {
    uint32_t mode;
    uint32_t length;
    union {
        struct { D2ClientInflateHuft* tree; uint32_t need; } code;
        struct { uint32_t get, distance; } copy;
        uint32_t literal;
    } sub;
    uint8_t literalBits, distanceBits;
    D2ClientInflateHuft* literalTree;
    D2ClientInflateHuft* distanceTree;
};
struct D2ClientInflateBlocks {
    uint32_t mode;
    union {
        D2ClientInflateCodes* codes; // retail +04, mode 6
        struct { uint32_t table, index; uint32_t* lengths; uint32_t bits; D2ClientInflateHuft* tree; } trees; // lengths +0c, bits +10, tree +14
    } sub;
    uint32_t last, bitCount, bitBuffer;
    D2ClientInflateHuft* workspace;
    D2ClientInflateWindow window;
};
void D2Client_ResetInflateBlocks(D2ClientInflateBlocks&, const D2ClientInflateAllocator&,
                                uint32_t& streamChecksum, uint32_t* previousChecksum);
int32_t D2Client_FreeInflateBlocks(D2ClientInflateBlocks*, const D2ClientInflateAllocator&,
                                 uint32_t& streamChecksum);
D2ClientInflateBlocks* D2Client_NewInflateBlocks(const D2ClientInflateAllocator&, uint32_t windowSize,
    uint32_t (*check)(uint32_t, const uint8_t*, uint32_t), uint32_t& streamChecksum);
D2ClientInflateCodes* D2Client_NewInflateCodes(const D2ClientInflateAllocator&, uint8_t literalBits,
    uint8_t distanceBits, D2ClientInflateHuft* literalTree, D2ClientInflateHuft* distanceTree);
