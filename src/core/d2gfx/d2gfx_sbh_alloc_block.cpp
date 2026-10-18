// src/core/d2gfx/d2gfx_sbh_alloc_block.cpp -- 1.13c D2Gfx.dll ___sbh_alloc_block, reimplemented from
// build-wasm/ghidra-d2gfx/decomp/0x4B87.c (alpha lane). Image base 0x6FA80000; every global is named at
// its retail RVA. Callees that are not recovered yet are host seams AFTER the retail arguments, which
// keep their order (the d2gfx_sbh_free_block pattern).
#include <cstdint>
#include <emscripten.h>

// --- D2Gfx __sbh_* state (the CRT small-block heap) -------------------------------------------
// File-local: the alloc side of this heap owns its own state (the d2gfx_sbh_free_block pattern).
static uint32_t g_sbh_entry;    // +0x1D8E0 DAT_6fa9d8e0: region record owning the last free
static uint32_t g_sbh_count;    // +0x1D8E4 DAT_6fa9d8e4: region count
static uint32_t g_sbh_regions;  // +0x1D8E8 DAT_6fa9d8e8: region-array base
static uint32_t g_sbh_last;     // +0x1D8F0 DAT_6fa9d8f0: last region handed out
static uint32_t g_sbh_seq;      // +0x1D8F8 DAT_6fa9d8f8: subregion index of the last free

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

struct D2GfxSbhAllocEnv {
    uint32_t (*alloc_new_region)(void);            // 0x046EB ___sbh_alloc_new_region
    int32_t  (*alloc_new_group)(uint32_t region);  // 0x047A2 ___sbh_alloc_new_group
};

// 1.13c D2Gfx+0x04B87 (764 B): ___sbh_alloc_block(size). Round the request up to a 16-byte footprint
// (size+0x17 & ~0xF) and derive its first bucket ((size+0x17)>>4 - 1), splitting the mask at bucket 0x20.
// Scan from the last-used region to the end of the region array for a region whose low/high bucket
// bitmaps serve the request (starting at the last-used region, then around the ring, then any region with
// a committed group; allocating a new region/group when none exists), select the first group whose
// low-then-high bitmap has a bit in the request's bucket range, and take the head of the first non-empty
// bucket at or above it. The block is unlinked from its bucket, the size remainder is split off (bucket
// clamped to 0x3F), and the allocation is marked by writing size+1 at the node and at the tail word. The
// bucket count at the group descriptor is decremented; when it was the last block of the region owning
// the last free, that cursor is cleared. Returns the user pointer (node+4) or 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2gfx_4b87(uint32_t size, const D2GfxSbhAllocEnv* env) {
    uint32_t req = (size + 0x17u) & 0xfffffff0u;
    int32_t firstBucket = (int32_t)((size + 0x17u) >> 4) - 1;
    uint32_t end = g_sbh_regions + g_sbh_count * 0x14u;
    uint8_t b8 = (uint8_t)firstBucket;
    uint32_t region = g_sbh_last;
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
    uint32_t r2 = g_sbh_regions;
    if (region == end) {
        while (r2 < g_sbh_last && ((P32(r2)[1] & hiMask) == 0 && (P32(r2)[0] & loMask) == 0))
            r2 += 0x14;
        region = r2;
        if (r2 == g_sbh_last) {
            while (r2 < end && P32(r2)[2] == 0) r2 += 0x14;
            uint32_t r3 = g_sbh_regions;
            region = r2;
            if (r2 == end) {
                while (r3 < g_sbh_last && P32(r3)[2] == 0) r3 += 0x14;
                region = r3;
                if (r3 == g_sbh_last) {
                    region = env->alloc_new_region();
                    if (region == 0) return 0;
                }
            }
            int32_t gi = env->alloc_new_group(region);
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
    g_sbh_last = region;
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
    if (count == 0 && region == g_sbh_entry && group == (int32_t)g_sbh_seq) g_sbh_entry = 0;
    P32(control)[0] = (uint32_t)group;
    return node + 4;
}
