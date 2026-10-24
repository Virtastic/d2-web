// src/core/d2game/d2game_sbh_alloc_block.cpp -- 1.13c D2Game.dll 0x093FC ___sbh_alloc_block, from the
// cached decompile (0x93FC.c). Same MSVC 2003 CRT function as D2MCPClient 0x3D23
// (src/core/realm/mcp_sbh_alloc.cpp); globals are host-owned pointers as in d2game_sbh_alloc_new_region.cpp:
// regions 0x6FD39FBC, count 0x6FD39FB8, last_region 0x6FD39FC4, entry 0x6FD39FB4, seq 0x6FD39FCC.
#include <cstdint>
#include <bit>
#include <emscripten.h>

struct D2GameSbh { uint32_t count, capacity, regions, heap, last_region, entry, seq; };
extern "C" D2GameSbh g_D2Game_sbh;  // defined in d2game_sbh_alloc_new_region.cpp
extern "C" uint32_t d2game_sbh_alloc_new_region();
extern "C" int32_t D2Game_9017_SbhAllocNewGroup(uint32_t region);
static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// 1.13c D2Game+0x093FC (764 B): ___sbh_alloc_block(size). Pick a region whose bucket bitmap can
// serve the request (starting at the last-used region, then around the ring, then any region with a
// committed group; allocating a new region/group when none exists), select the first group whose
// low-then-high bitmap has a bit in the request's bucket range, and take the head of the first
// non-empty bucket at or above it. The block is unlinked from its bucket, the size remainder is split
// off (bucket clamped to 0x3F), and the allocation is marked by writing size+1 at the node and at the
// tail word. The bucket count at the group descriptor is decremented; when it was the last block of
// the region owning the last free, that cursor is cleared. Returns the user pointer (node+4) or 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2game_sbh_alloc_block(uint32_t size) {
    D2GameSbh* env = &g_D2Game_sbh;
    uint32_t rounded = size + 0x17u;
    uint32_t req = rounded & 0xfffffff0u;
    // Retail converts the wrapped 32-bit sum back to signed before its arithmetic shift.
    int32_t firstBucket = (std::bit_cast<int32_t>(rounded) >> 4) - 1;
    uint32_t end = env->regions + env->count * 0x14u;
    uint8_t b8 = (uint8_t)firstBucket;
    uint32_t region = env->last_region;
    uint32_t loMask, hiMask;
    if (firstBucket < 0x20) {
        loMask = 0xffffffffu >> (b8 & 31);
        hiMask = 0xffffffffu;
    } else {
        loMask = 0;
        hiMask = 0xffffffffu >> ((uint8_t)(b8 - 0x20) & 31);
    }
    while (region < end && ((P32(region)[1] & hiMask) == 0 && (P32(region)[0] & loMask) == 0))
        region += 0x14;
    uint32_t r2 = env->regions;
    if (region == end) {
        while (r2 < env->last_region && ((P32(r2)[1] & hiMask) == 0 && (P32(r2)[0] & loMask) == 0))
            r2 += 0x14;
        region = r2;
        if (r2 == env->last_region) {
            while (r2 < end && P32(r2)[2] == 0) r2 += 0x14;
            uint32_t r3 = env->regions;
            region = r2;
            if (r2 == end) {
                while (r3 < env->last_region && P32(r3)[2] == 0) r3 += 0x14;
                region = r3;
                if (r3 == env->last_region) {
                    region = d2game_sbh_alloc_new_region();
                    if (region == 0) return 0;
                }
            }
            int32_t gi = D2Game_9017_SbhAllocNewGroup(region);
            P32(P32(region + 0x10)[0])[0] = (uint32_t)gi;
            if (P32(P32(region + 0x10)[0])[0] == 0xffffffffu) return 0;
        }
    }
    uint32_t control = P32(region + 0x10)[0];
    int32_t group = (int32_t)P32(control)[0];
    if (group == -1 ||
        ((P32(control + 0xC4 + (uint32_t)group * 4)[0] & hiMask) == 0 &&
         (P32(control + 0x44 + (uint32_t)group * 4)[0] & loMask) == 0)) {
        group = 0;
        uint32_t p = control + 0x44;
        if ((P32(control + 0xC4)[0] & hiMask) == 0 && (P32(p)[0] & loMask) == 0) {
            uint32_t q;
            do {
                q = p + 0x84;
                group++;
                p += 4;
            } while ((P32(q)[0] & hiMask) == 0 && (P32(p)[0] & loMask) == 0);
        }
    }
    uint32_t bucketBase = control + (uint32_t)group * 0x204u + 0x144u;
    int32_t bucket = 0;
    uint32_t bits = P32(control + 0x44 + (uint32_t)group * 4)[0] & loMask;
    if (bits == 0) {
        bits = P32(control + 0xC4 + (uint32_t)group * 4)[0] & hiMask;
        bucket = 0x20;
    }
    while ((int32_t)bits >= 0) { bucket++; bits <<= 1; }
    uint32_t node = P32(bucketBase + (uint32_t)bucket * 8 + 4)[0];
    int32_t rem = (int32_t)P32(node)[0] - (int32_t)req;
    int32_t newBucket = (rem >> 4) - 1;
    if (newBucket > 0x3F) newBucket = 0x3F;
    env->last_region = region;
    if (newBucket != bucket) {
        if (P32(node)[1] == P32(node)[2]) {
            uint8_t* tally = P8(control + (uint32_t)bucket + 4);
            if (bucket < 0x20) {
                uint32_t mask = ~(0x80000000u >> ((uint8_t)bucket & 31));
                P32(control + 0x44 + (uint32_t)group * 4)[0] &= mask;
                (*tally)--;
                if (*tally == 0) P32(region)[0] &= mask;
            } else {
                uint32_t mask = ~(0x80000000u >> ((uint8_t)(bucket - 0x20) & 31));
                P32(control + 0xC4 + (uint32_t)group * 4)[0] &= mask;
                (*tally)--;
                if (*tally == 0) P32(region)[1] &= mask;
            }
        }
        *(uint32_t*)(uintptr_t)(P32(node)[2] + 4) = P32(node)[1];
        *(uint32_t*)(uintptr_t)(P32(node)[1] + 8) = P32(node)[2];
        if (rem != 0) {
            uint32_t list = bucketBase + (uint32_t)newBucket * 8;
            P32(node)[2] = list;
            P32(node)[1] = P32(list + 4)[0];
            P32(list + 4)[0] = node;
            *P32(P32(node)[1] + 8) = node;
            if (P32(node)[1] == P32(node)[2]) {
                uint8_t* tally = P8(control + (uint32_t)newBucket + 4);
                uint8_t was = *tally;
                *tally = (uint8_t)(was + 1);
                uint8_t nb = (uint8_t)newBucket;
                if (newBucket < 0x20) {
                    if (was == 0) P32(region)[0] |= 0x80000000u >> (nb & 31);
                    P32(control + 0x44 + (uint32_t)group * 4)[0] |= 0x80000000u >> (nb & 31);
                } else {
                    if (was == 0) P32(region)[1] |= 0x80000000u >> ((uint8_t)(nb - 0x20) & 31);
                    P32(control + 0xC4 + (uint32_t)group * 4)[0] |=
                        0x80000000u >> ((uint8_t)(nb - 0x20) & 31);
                }
            }
        }
    }
    if (rem != 0) {
        P32(node)[0] = (uint32_t)rem;
        *(uint32_t*)(uintptr_t)(node + (uint32_t)rem - 4) = (uint32_t)rem;
    }
    node += (uint32_t)rem;
    P32(node)[0] = req + 1;
    *(uint32_t*)(uintptr_t)(node + req - 4) = req + 1;
    int32_t count = (int32_t)P32(bucketBase)[0];
    P32(bucketBase)[0] = (uint32_t)(count + 1);
    if (count == 0 && region == env->entry && group == (int32_t)env->seq) env->entry = 0;
    P32(control)[0] = (uint32_t)group;
    return node + 4;
}
