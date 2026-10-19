#pragma once
#include "inflate_lifecycle.h"

// Stream next_in/avail_in/total_in/msg (+00/+04/+08/+18) as native views.
struct D2ClientInflateInput {
    const uint8_t* next;
    uint32_t available, total;
    const char* message;
};
// Caller guarantees >=10 input bytes and >=258 contiguous window bytes.
// Huffman child tables use entry-relative value.base offsets, as in retail.
int32_t D2Client_InflateFast(uint32_t literalBits, uint32_t distanceBits,
    const D2ClientInflateHuft* literalTree, const D2ClientInflateHuft* distanceTree,
    D2ClientInflateBlocks&, D2ClientInflateInput&);
