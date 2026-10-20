#pragma once
#include "inflate_lifecycle.h"

int32_t D2Client_BuildInflateTree(const uint32_t* lengths, uint32_t count,
    uint32_t simple, const uint32_t* bases, const uint32_t* extra,
    D2ClientInflateHuft*& tree, uint32_t& bits, D2ClientInflateHuft* workspace,
    uint32_t& used, uint32_t* sorted);
int32_t D2Client_InflateDynamicTrees(uint32_t literals, uint32_t distances,
    const uint32_t* lengths, uint32_t& literalBits, uint32_t& distanceBits,
    D2ClientInflateHuft*& literalTree, D2ClientInflateHuft*& distanceTree,
    D2ClientInflateHuft* workspace, const D2ClientInflateAllocator&, const char*& message);
int32_t D2Client_InflateBitTree(const uint32_t* lengths, uint32_t& bits,
    D2ClientInflateHuft*& tree, D2ClientInflateHuft* workspace,
    const D2ClientInflateAllocator&, const char*& message);

inline constexpr uint32_t lengthBase[] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258,0,0};
inline constexpr uint32_t lengthExtra[] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,112,112};
inline constexpr uint32_t distanceBase[] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
inline constexpr uint32_t distanceExtra[] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
