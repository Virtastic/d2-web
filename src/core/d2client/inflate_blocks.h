#pragma once
#include "inflate_fast.h"
#include "stream_algorithms_wave_a.h"
#include "inflate_lifecycle.h"

int32_t D2Client_InflateBlocks(D2ClientInflateBlocks&, D2ClientInflateInput&,
    D2ClientInflateOutput&, const D2ClientInflateAllocator&, int32_t result);

// The fixed-block trees: retail keeps them as static module data (bits 0xF8CC8/0xF8CCC, literal tree 0xF8CD0,
// distance tree 0xF9CD0) and hands their addresses to the new-codes call; these are the same 512 + 32 entries.
extern uint32_t D2Client_FixedLiteralBits;
extern uint32_t D2Client_FixedDistanceBits;
extern D2ClientInflateHuft D2Client_FixedLiteralTree[512], D2Client_FixedDistanceTree[32];

// Test seam (FN probes): replaces the callees; null members fall back to the real functions.
struct D2ClientInflateBlocksCallees {
    int32_t (*codes)(D2ClientInflateBlocks&, D2ClientInflateInput&, D2ClientInflateOutput&, int32_t);
    int32_t (*flush)(D2ClientInflateWindow&, D2ClientInflateOutput&, int32_t);
    D2ClientInflateCodes* (*newCodes)(const D2ClientInflateAllocator&, uint8_t, uint8_t, D2ClientInflateHuft*, D2ClientInflateHuft*);
    int32_t (*bitTree)(const uint32_t*, uint32_t&, D2ClientInflateHuft*&, D2ClientInflateHuft*,
        const D2ClientInflateAllocator&, const char*&);
    int32_t (*dynamicTrees)(uint32_t, uint32_t, const uint32_t*, uint32_t&, uint32_t&, D2ClientInflateHuft*&,
        D2ClientInflateHuft*&, D2ClientInflateHuft*, const D2ClientInflateAllocator&, const char*&);
};
void D2Client_SetInflateBlocksCallees(const D2ClientInflateBlocksCallees*);
