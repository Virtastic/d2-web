// src/core/realm/mcp_sbh_alloc.cpp -- five UNMAPPED 1.13c D2MCPClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane). These are the MSVC 2003
// Release CRT small-block heap's allocation half; the free half (0x356F) and the state globals live in
// mcp_sbh_free.cpp. Image base 0x6FA20000; every global is named at its retail RVA. Every kernel32
// import (the wasm module has no PE import table) is a host seam AFTER the retail arguments, which
// keep their order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern uint32_t g_sbh_entry;        // +0x1224C  DAT_6fa3224c
extern uint32_t g_sbh_count;        // +0x12250  DAT_6fa32250
extern uint32_t g_sbh_regions;      // +0x12254  DAT_6fa32254
extern uint32_t g_sbh_max;          // +0x12258  DAT_6fa32258
extern uint32_t g_sbh_last_region;  // +0x1225C  DAT_6fa3225c
extern uint32_t g_sbh_capacity;     // +0x12260  DAT_6fa32260
extern uint32_t g_sbh_seq;          // +0x12264  DAT_6fa32264
extern uint32_t g_sbh_heap;         // +0x124A0  DAT_6fa324a0

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

struct D2McpSbhAllocEnv {
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes);              // HeapAlloc
    uint32_t (*heap_realloc)(uint32_t heap, uint32_t flags, uint32_t mem, uint32_t bytes);  // HeapReAlloc
    int32_t  (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);               // HeapFree
    uint32_t (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect);
};

// 1.13c D2MCPClient+0x034FC (72 B): ___sbh_heap_init(dwMaximum). Allocate the 0x140-byte region table
// from the Win32 heap, then reset the six sbh globals: no region yet, none allocated, the "last region"
// cursor at the table head, the caller's maximum request size, and room for 0x10 regions. Returns 0
// only when the table allocation fails, 1 otherwise.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_34fc(uint32_t maximum, const D2McpSbhAllocEnv* env) {
    uint32_t regions = env->heap_alloc(g_sbh_heap, 0, 0x140);
    g_sbh_regions = regions;
    if (regions == 0) return 0;
    g_sbh_entry = 0;
    g_sbh_count = 0;
    g_sbh_last_region = regions;
    g_sbh_max = maximum;
    g_sbh_capacity = 0x10;
    return 1;
}

// 1.13c D2MCPClient+0x03887 (183 B): ___sbh_alloc_new_region. When the region table is full, grow it
// by 0x10 entries ((capacity*5 + 0x50) dwords). Carve the next 0x14-byte region record and allocate its
// control block (0x41C4 bytes, zeroed) and its 1 MB reservation (MEM_RESERVE, PAGE_READWRITE). On
// success clear the record (no free bucket bits, no committed subregion, no base), bump the region
// count and mark the control block as having no current group (0xFFFFFFFF). On either allocation
// failure, free what was taken and return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_3887(const D2McpSbhAllocEnv* env) {
    if (g_sbh_count == g_sbh_capacity) {
        uint32_t grown = env->heap_realloc(g_sbh_heap, 0, g_sbh_regions,
                                           (g_sbh_capacity * 5u + 0x50u) * 4u);
        if (grown == 0) return 0;
        g_sbh_capacity += 0x10;
        g_sbh_regions = grown;
    }
    uint32_t region = g_sbh_regions + g_sbh_count * 0x14u;
    uint32_t control = env->heap_alloc(g_sbh_heap, 8, 0x41C4);
    P32(region + 0x10)[0] = control;
    if (control != 0) {
        uint32_t base = env->virtual_alloc(0, 0x100000, 0x2000, 4);
        P32(region + 0xC)[0] = base;
        if (base != 0) {
            P32(region + 8)[0] = 0xffffffffu;
            P32(region)[0] = 0;
            P32(region + 4)[0] = 0;
            g_sbh_count += 1;
            P32(control)[0] = 0xffffffffu;
            return region;
        }
        env->heap_free(g_sbh_heap, 0, control);
    }
    return 0;
}

// 1.13c D2MCPClient+0x0393E (262 B): ___sbh_alloc_new_group(region). Pick the first zero bit of the
// region's committed-subregion mask (bits are consumed most-significant first), initialise the 0x3F
// bucket lists for that group's bucket descriptor array, and commit the 0x8000-byte group with
// VirtualAlloc(MEM_COMMIT, PAGE_READWRITE). Each of the eight 0x1000-byte subregions is then seeded
// with one free block spanning 0xFF0 bytes, its head/tail size words and the links keeping the
// subregions chained; the outer ends are linked to bucket 0x3F's list. Finally clear the group's
// low-bucket bitmap to 0, set its high-bucket bitmap to 1, bump the committed-group count (setting
// region[1] bit 0 on the first group) and clear the chosen bit in region[2]. Returns the group index
// or -1 when the commit fails.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_393e(uint32_t region, const D2McpSbhAllocEnv* env) {
    uint32_t control = P32(region + 0x10)[0];
    int32_t group = 0;
    uint32_t bits = P32(region + 8)[0];
    while ((int32_t)bits >= 0) { group++; bits <<= 1; }
    uint32_t buckets = (uint32_t)group * 0x204u + 0x144u + control;
    uint32_t it = buckets;
    for (int32_t n = 0x3F; n != 0; --n) {
        P32(it + 8)[0] = it;
        P32(it + 4)[0] = it;
        it += 8;
    }
    uint32_t lpAddress = (uint32_t)group * 0x8000u + P32(region + 0xC)[0];
    if (env->virtual_alloc(lpAddress, 0x8000, 0x1000, 4) == 0) return -1;
    if (lpAddress <= lpAddress + 0x7000u) {
        uint32_t p = lpAddress + 0x10;
        uint32_t count = ((lpAddress + 0x7000u - lpAddress) >> 12) + 1;
        do {
            P32(p - 8)[0] = 0xffffffffu;
            P32(p + 0xFEC)[0] = 0xffffffffu;
            P32(p)[0] = p + 0xFFC;
            P32(p - 4)[0] = 0xFF0;
            P32(p + 4)[0] = p - 0x1004;
            P32(p + 0xFE8)[0] = 0xFF0;
            p += 0x1000;
            count--;
        } while (count != 0);
    }
    P32(buckets + 0x1FC)[0] = lpAddress + 0xC;
    P32(lpAddress + 0x14)[0] = buckets + 0x1F8;
    P32(buckets + 0x200)[0] = lpAddress + 0x700C;
    P32(lpAddress + 0x7010)[0] = buckets + 0x1F8;
    P32(control + 0x44 + (uint32_t)group * 4)[0] = 0;
    P32(control + 0xC4 + (uint32_t)group * 4)[0] = 1;
    uint8_t committed = *P8(control + 0x43);
    *P8(control + 0x43) = (uint8_t)(committed + 1);
    if (committed == 0) P32(region + 4)[0] |= 1;
    P32(region + 8)[0] &= ~(0x80000000u >> ((uint8_t)group & 31));
    return group;
}

// 1.13c D2MCPClient+0x03A44 (735 B): ___sbh_resize_block(region, block, requested). Round the request
// up to 16 bytes + 0x17 and compare it with the block's current size (the word at block-4 minus the
// in-use bit). Growing: fail when the next block is allocated or the two do not cover the request;
// otherwise unlink the free next block, split off the remainder as a new free block (bucket by size,
// clamped to 0x3F) and write both size words. Shrinking: split off the tail; when the next block is
// free it is unlinked from its bucket and folded into the tail before the tail is linked. Bucket
// bitmaps/tallies are maintained at control[0]/control[1] (buckets 0..0x1F via +0x44, 0x20..0x3F via
// +0xC4). Returns 1 when resized, 0 when the block cannot grow.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_mcp_3a44(uint32_t region, uint32_t block, uint32_t requested) {
    uint32_t control = P32(region)[4];
    uint32_t group = (block - P32(region)[3]) >> 15;
    uint32_t buckets = group * 0x204u + 0x144u + control;
    uint32_t req = (requested + 0x17u) & 0xfffffff0u;
    int32_t curSize = (int32_t)P32(block - 4)[0] - 1;
    uint32_t nextHdr = (uint32_t)((int32_t)P32(block - 4)[0] - 5 + (int32_t)block);
    uint32_t nextSize = P32(nextHdr)[0];
    if (curSize < (int32_t)req) {
        if ((nextSize & 1) != 0 || (int32_t)(nextSize + (uint32_t)curSize) < (int32_t)req) return 0;
        uint32_t b = ((int32_t)nextSize >> 4) - 1;
        if (b > 0x3F) b = 0x3F;
        if (P32(nextHdr)[1] == P32(nextHdr)[2]) {
            uint8_t* tally = P8(control + b + 4);
            if (b < 0x20) {
                uint32_t mask = ~(0x80000000u >> ((uint8_t)b & 31));
                P32(control + 0x44 + group * 4)[0] &= mask;
                (*tally)--;
                if (*tally == 0) P32(region)[0] &= mask;
            } else {
                uint32_t mask = ~(0x80000000u >> ((uint8_t)(b - 0x20) & 31));
                P32(control + 0xC4 + group * 4)[0] &= mask;
                (*tally)--;
                if (*tally == 0) P32(region)[1] &= mask;
            }
        }
        *(uint32_t*)(uintptr_t)(P32(nextHdr)[2] + 4) = P32(nextHdr)[1];
        *(uint32_t*)(uintptr_t)(P32(nextHdr)[1] + 8) = P32(nextHdr)[2];
        int32_t left = (int32_t)nextSize + (curSize - (int32_t)req);
        if (left > 0) {
            uint32_t b2 = ((uint32_t)left >> 4) - 1;
            if (b2 > 0x3F) b2 = 0x3F;
            uint32_t h = block - 4 + req;
            uint32_t list = buckets + b2 * 8;
            P32(h)[1] = P32(list + 4)[0];
            P32(h)[2] = list;
            P32(list + 4)[0] = h;
            *P32(P32(h)[1] + 8) = h;
            if (P32(h)[1] == P32(h)[2]) {
                uint8_t* tally = P8(control + b2 + 4);
                uint8_t was = *tally;
                *tally = (uint8_t)(was + 1);
                if (b2 < 0x20) {
                    if (was == 0) P32(region)[0] |= 0x80000000u >> (b2 & 31);
                    P32(control + 0x44 + group * 4)[0] |= 0x80000000u >> (b2 & 31);
                } else {
                    if (was == 0) P32(region)[1] |= 0x80000000u >> ((b2 - 0x20) & 31);
                    P32(control + 0xC4 + group * 4)[0] |= 0x80000000u >> ((b2 - 0x20) & 31);
                }
            }
            P32(h)[0] = (uint32_t)left;
            *(uint32_t*)(uintptr_t)(h + (uint32_t)left - 4) = (uint32_t)left;
        }
        P32(block - 4)[0] = req + 1;
        *(uint32_t*)(uintptr_t)(block - 8 + req) = req + 1;
    } else if ((int32_t)req < curSize) {
        uint32_t left = (uint32_t)(curSize - (int32_t)req);
        P32(block - 4)[0] = req + 1;
        uint32_t h = block - 4 + req;
        uint32_t b = (left >> 4) - 1;
        if (b > 0x3F) b = 0x3F;
        P32(h - 4)[0] = req + 1;
        if ((nextSize & 1) == 0) {
            uint32_t nb = ((int32_t)nextSize >> 4) - 1;
            if (nb > 0x3F) nb = 0x3F;
            if (P32(nextHdr)[1] == P32(nextHdr)[2]) {
                uint8_t* tally = P8(control + nb + 4);
                if (nb < 0x20) {
                    uint32_t mask = ~(0x80000000u >> ((uint8_t)nb & 31));
                    P32(control + 0x44 + group * 4)[0] &= mask;
                    (*tally)--;
                    if (*tally == 0) P32(region)[0] &= mask;
                } else {
                    uint32_t mask = ~(0x80000000u >> ((uint8_t)(nb - 0x20) & 31));
                    P32(control + 0xC4 + group * 4)[0] &= mask;
                    (*tally)--;
                    if (*tally == 0) P32(region)[1] &= mask;
                }
            }
            *(uint32_t*)(uintptr_t)(P32(nextHdr)[2] + 4) = P32(nextHdr)[1];
            *(uint32_t*)(uintptr_t)(P32(nextHdr)[1] + 8) = P32(nextHdr)[2];
            left += nextSize;
            b = (left >> 4) - 1;
            if (b > 0x3F) b = 0x3F;
        }
        uint32_t list = buckets + b * 8;
        P32(h)[2] = list;
        P32(h)[1] = P32(list + 4)[0];
        P32(list + 4)[0] = h;
        *P32(P32(h)[1] + 8) = h;
        if (P32(h)[1] == P32(h)[2]) {
            uint8_t* tally = P8(control + b + 4);
            uint8_t was = *tally;
            *tally = (uint8_t)(was + 1);
            if (b < 0x20) {
                if (was == 0) P32(region)[0] |= 0x80000000u >> (b & 31);
                P32(control + 0x44 + group * 4)[0] |= 0x80000000u >> (b & 31);
            } else {
                if (was == 0) P32(region)[1] |= 0x80000000u >> ((b - 0x20) & 31);
                P32(control + 0xC4 + group * 4)[0] |= 0x80000000u >> ((b - 0x20) & 31);
            }
        }
        P32(h)[0] = left;
        *(uint32_t*)(uintptr_t)(h + left - 4) = left;
    }
    return 1;
}

// 1.13c D2MCPClient+0x03D23 (764 B): ___sbh_alloc_block(size). Pick a region whose bucket bitmap can
// serve the request (starting at the last-used region, then around the ring, then any region with a
// committed group; allocating a new region/group when none exists), select the first group whose
// low-then-high bitmap has a bit in the request's bucket range, and take the head of the first
// non-empty bucket at or above it. The block is unlinked from its bucket, the size remainder is split
// off (bucket clamped to 0x3F), and the allocation is marked by writing size+1 at the node and at the
// tail word. The bucket count at the group descriptor is decremented; when it was the last block of
// the region owning the last free, that cursor is cleared. Returns the user pointer (node+4) or 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_3d23(uint32_t size, const D2McpSbhAllocEnv* env) {
    uint32_t req = (size + 0x17u) & 0xfffffff0u;
    int32_t firstBucket = (int32_t)((size + 0x17u) >> 4) - 1;
    uint32_t end = g_sbh_regions + g_sbh_count * 0x14u;
    uint8_t b8 = (uint8_t)firstBucket;
    uint32_t region = g_sbh_last_region;
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
        while (r2 < g_sbh_last_region && ((P32(r2)[1] & hiMask) == 0 && (P32(r2)[0] & loMask) == 0))
            r2 += 0x14;
        region = r2;
        if (r2 == g_sbh_last_region) {
            while (r2 < end && P32(r2)[2] == 0) r2 += 0x14;
            uint32_t r3 = g_sbh_regions;
            region = r2;
            if (r2 == end) {
                while (r3 < g_sbh_last_region && P32(r3)[2] == 0) r3 += 0x14;
                region = r3;
                if (r3 == g_sbh_last_region) {
                    region = d2_mcp_3887(env);
                    if (region == 0) return 0;
                }
            }
            int32_t gi = d2_mcp_393e(region, env);
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
    g_sbh_last_region = region;
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
