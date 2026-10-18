// src/core/gfx/d2ddraw_batch_2e23.cpp -- twelve UNMAPPED 1.13c D2DDraw.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2ddraw/decomp/0x<RVA>.c (alpha lane: no spec/vector yet). Retail
// base 0x6F8C0000; every RVA below is retail's and every module global is named at its retail RVA.
//
// Rows 0x2E23-0x32BF are the MSVC 2003 (VC7.1) Release CRT small-block heap's allocation half, the
// same code as src/core/realm/mcp_sbh_alloc.cpp (0x3887/0x393E/0x3A44/0x3D23) with D2DDraw's own
// globals. 0x35C0 is the CRT memcpy (its backward path makes it memmove). 0x3942/0x39D6 are the SEH
// unwind walker and its bookkeeping fragment. 0x3C17/0x3C27 are the critical-section initialiser. The
// three tlocinfo rows (0x3CBB/0x3D8B/0x3E4C) free and refresh the per-thread locale record.
// Every kernel32 import and every unported internal callee is a host seam AFTER the retail arguments,
// which keep their order.
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include <Windows.h>   // CRITICAL_SECTION / InitializeCriticalSection: host (kernel) state

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// Earlier D2DDraw batches: __getptd (0x1BBE), __lock (0x1ACC), the __unlock(0xC) fragment (0x3E7E).
extern "C" void* d2_fn_d2ddraw_1bbe(void);
extern "C" void d2ddraw_sub_3E7E(void);
void d2ddraw_lock(int idx);

// --- CRT small-block heap state, all zero until __sbh_heap_init (a row not in this batch) ----------
static uint32_t g_sbh_entry;        // +0x104C0  DAT_6f8d04c0: region owning the last free
static uint32_t g_sbh_count;        // +0x104C4  DAT_6f8d04c4: regions in use
static uint32_t g_sbh_regions;      // +0x104C8  DAT_6f8d04c8: region-array base
static uint32_t g_sbh_last_region;  // +0x104D0  DAT_6f8d04d0: last region handed out
static uint32_t g_sbh_capacity;     // +0x104D4  DAT_6f8d04d4: region slots allocated
static uint32_t g_sbh_seq;          // +0x104D8  DAT_6f8d04d8: subregion index of the last free
static uint32_t g_sbh_heap;         // +0x104DC  DAT_6f8d04dc: Win32 heap handle

struct D2DDrawSbhEnv {
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes);                    // HeapAlloc
    uint32_t (*heap_realloc)(uint32_t heap, uint32_t flags, uint32_t mem, uint32_t bytes);    // HeapReAlloc
    int32_t  (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);                     // HeapFree
    uint32_t (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect); // VirtualAlloc
};

// 1.13c D2DDraw+0x02E23 (183 B): ___sbh_alloc_new_region. When the region table is full, grow it by
// 0x10 entries ((capacity*5 + 0x50) dwords). Carve the next 0x14-byte region record, allocate its
// control block (0x41C4 bytes from the heap) and reserve its 1 MB address range (MEM_RESERVE,
// PAGE_READWRITE). On success the record is cleared (no free buckets, no committed group, no base),
// the region count is bumped and the control block's current group is set to 0xFFFFFFFF. On either
// allocation failure, free what was taken and return 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2ddraw_sbh_alloc_new_region(const D2DDrawSbhEnv* env) {
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

// 1.13c D2DDraw+0x02EDA (262 B): ___sbh_alloc_new_group(region). Pick the first zero bit of the
// region's committed-subregion mask (most-significant first), initialise the 0x3F bucket lists of the
// group's bucket descriptor array, and commit the 0x8000-byte group (MEM_COMMIT, PAGE_READWRITE). Each
// of the eight 0x1000-byte subregions is seeded with one 0xFF0-byte free block, head/tail size words
// and the links chaining them; the outer ends attach to bucket 0x3F. Finally clear the group's
// low-bucket bitmap, set its high-bucket bitmap to 1, bump the committed-group count (region[1] bit 0
// on the first group) and clear the chosen bit in region[2]. Returns the group index, or -1 when the
// commit fails.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2ddraw_sbh_alloc_new_group(uint32_t region, const D2DDrawSbhEnv* env) {
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

// 1.13c D2DDraw+0x02FE0 (735 B): ___sbh_resize_block(region, block, requested). Round the request up
// to (requested + 0x17) & ~15 and compare with the block's current size (block-4 minus the in-use
// bit). Growing: fail when the next block is allocated or the two do not cover the request; otherwise
// unlink the free next block, split off the remainder as a new free block (bucket clamped to 0x3F) and
// write both size words. Shrinking: split off the tail; when the next block is free, unlink it from its
// bucket and fold it into the tail before the tail is linked. Bucket bitmaps/tallies live at the
// control block's [0]/[1] (buckets 0..0x1F via +0x44, 0x20..0x3F via +0xC4). Returns 1 when resized,
// 0 when the block cannot grow.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2ddraw_sbh_resize_block(uint32_t region, uint32_t block, uint32_t requested) {
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

// 1.13c D2DDraw+0x032BF (764 B): ___sbh_alloc_block(size). Pick a region whose bucket bitmap serves
// the request (from the last-used region, then around the ring, then any region with a committed
// group; allocating a new region/group when none exists), select the first group whose low-then-high
// bitmap has a bit in the request's bucket range, and take the head of the first non-empty bucket at
// or above it. Unlink that block, split off the remainder (bucket clamped to 0x3F), and mark the
// allocation with size+1 at the node and at the tail word. Decrement the bucket count; when it was the
// last block of the region owning the last free, clear that cursor. Returns the user pointer (node+4)
// or 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2ddraw_sbh_alloc_block(uint32_t size, const D2DDrawSbhEnv* env) {
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
                    region = d2ddraw_sbh_alloc_new_region(env);
                    if (region == 0) return 0;
                }
            }
            int32_t gi = d2ddraw_sbh_alloc_new_group(region, env);
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

// 1.13c D2DDraw+0x035C0 (672 B): _memcpy. The decompile's backward path (source below destination
// and the ranges overlapping) makes this the overlap-safe copy, i.e. memmove; the forward path is the
// word-at-a-time copy. The host's memmove has the same contract for every length and alignment, so it
// is the whole rule. Returns the destination base.
extern "C"
EMSCRIPTEN_KEEPALIVE void* d2ddraw_sub_35C0(void* dst, const void* src, size_t size) {
    return std::memmove(dst, src, size);
}

// --- SEH unwind state -----------------------------------------------------------------------------
static uint32_t g_exception_list;   // FS:[0] chain head (single-threaded here)
static uint32_t g_seh_e2b4;         // +0xE2B4  DAT_6f8ce2b4: EAX of the unwind fragment
static uint32_t g_seh_e2b8;         // +0xE2B8  DAT_6f8ce2b8: [EBP+8] of the unwind fragment
static uint32_t g_seh_e2bc;         // +0xE2BC  DAT_6f8ce2bc: EBP of the unwind fragment

// 1.13c D2DDraw+0x039D6 (24 B): the SEH bookkeeping fragment __local_unwind2 runs before a __finally
// handler. Its operands are the caller's registers, not an argument list -- EAX is the handler
// address, [EBP+8] the 0x101 the caller pushed, EBP the caller's frame -- so they arrive as arguments.
// Body: +0xE2B8 = [EBP+8], +0xE2B4 = EAX, +0xE2BC = EBP. No reader in the image.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_sub_39D6(uint32_t handler, uint32_t arg, uint32_t frame) {
    g_seh_e2b8 = arg;
    g_seh_e2b4 = handler;
    g_seh_e2bc = frame;
}

// 1.13c D2DDraw+0x03942 (104 B): __local_unwind2(frame, target). Walk the frame's table (frame+8)
// from index frame+0xC until the index is -1 or equals the target. Each 12-byte record is
// {next index, filter, handler}; a zero filter first runs 0x39D6 with the pushed selector 0x101, then
// the handler. The SEH chain head is published for the walk and restored after. `caller_ebp` is the
// frame word the abandoned 0x39D6 call records.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_local_unwind2(uint8_t* frame, int32_t target, uint32_t caller_ebp) {
    uint32_t saved_list = g_exception_list;
    g_exception_list = (uint32_t)(uintptr_t)frame;
    for (;;) {
        uint32_t table = *(uint32_t*)(frame + 8);
        uint32_t idx = *(uint32_t*)(frame + 0xc);
        if (idx == 0xFFFFFFFFu || (int32_t)idx == target) break;
        uint8_t* rec = P8(table + idx * 12);
        *(uint32_t*)(frame + 0xc) = *(uint32_t*)rec;
        if (*(uint32_t*)(rec + 4) == 0) {
            uint32_t handler = *(uint32_t*)(rec + 8);
            d2ddraw_sub_39D6(handler, 0x101, caller_ebp);
            ((void (*)(void))(uintptr_t)handler)();
        }
    }
    g_exception_list = saved_list;
}

// --- critical-section initialiser -----------------------------------------------------------------
typedef int32_t (*D2DDrawInitCritSecFn)(void* cs, uint32_t spin);
static D2DDrawInitCritSecFn g_init_crit_sec;   // +0xF204  DAT_6f8cf204: resolved initialiser
static uint32_t g_os_platform;                 // +0xEEE4  DAT_6f8ceee4: 1 = NT

struct D2DDrawInitCritSecEnv {
    uint32_t (*get_module_handle)(const char* name);                     // GetModuleHandleA
    int32_t  (*get_proc_address)(uint32_t module, const char* name);     // GetProcAddress
};

// 1.13c D2DDraw+0x03C17 (16 B, stdcall @8): ___crtInitCritSecNoSpinCount. Initialize the critical
// section, ignore the spin word and return 1. Retail pops both stack words (`ret 8`), so it matches
// the kernel32 initialiser's two-argument call shape.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2ddraw_crt_init_crit_sec_no_spin(void* cs, uint32_t spin) {
    (void)spin;
    InitializeCriticalSection((LPCRITICAL_SECTION)cs);
    return 1;
}

// 1.13c D2DDraw+0x03C27 (103 B): ___crtInitCritSecAndSpinCount(cs, spin). Resolve
// InitializeCriticalSectionAndSpinCount once: on NT (platform word 1) or when kernel32 does not export
// it, the no-spin thunk 0x3C17 is stored instead. Then call the stored initialiser with both words.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_crt_init_crit_sec_and_spin(void* cs, uint32_t spin,
                                                             const D2DDrawInitCritSecEnv* env) {
    if (g_init_crit_sec == nullptr) {
        D2DDrawInitCritSecFn resolved = nullptr;
        if (g_os_platform != 1) {
            uint32_t mod = env->get_module_handle("kernel32.dll");
            if (mod != 0)
                resolved = (D2DDrawInitCritSecFn)(uintptr_t)
                    env->get_proc_address(mod, "InitializeCriticalSectionAndSpinCount");
        }
        g_init_crit_sec = resolved != nullptr ? resolved : d2ddraw_crt_init_crit_sec_no_spin;
    }
    g_init_crit_sec(cs, spin);
}

// --- per-thread locale info -----------------------------------------------------------------------
// +0xEF230/0xEF234/0x10278/0x1027C/0x10280 and +0xE430 are static locale records the free path must
// not free; they are modelled as opaque storage and compared by identity (exactly the retail test).
static uint32_t g_lc_time_static_ef230[0x40];   // +0xEF230  DAT_6f8cf230
static uint32_t g_clocale_static_ef234;         // +0xEF234  DAT_6f8cf234 (a pointer-valued global)
static uint32_t g_lconv_static_10278[0x40];     // +0x10278  DAT_6f8d0278
static uint32_t g_lconv_static_1027c[0x40];     // +0x1027C  DAT_6f8d027c
static uint32_t g_lconv_static_10280[0x40];     // +0x10280  DAT_6f8d0280
static uint32_t g_tlocinfo_static_e430[0x60];   // +0xE430  DAT_6f8ce430

// +0xE484 PTR_DAT_6f8ce484: the image's default tlocinfo. It is a static record whose time-info
// pointer (+0x4C) exists before any thread asks for a locale, so the backing store is filled in once.
static uint32_t g_default_tlocinfo_store[0x60];
static uint32_t g_default_time_store[0x40];
static uint32_t* g_tlocinfo_default;            // value of PTR_DAT_6f8ce484
static uint32_t* default_tlocinfo(void) {
    if (g_tlocinfo_default == nullptr) {
        g_default_tlocinfo_store[0x13] = (uint32_t)(uintptr_t)g_default_time_store;
        g_tlocinfo_default = g_default_tlocinfo_store;
    }
    return g_tlocinfo_default;
}

struct D2DDrawTlocinfoEnv {
    void (*free_lconv_mon)(uint32_t locale);   // 0x4B1D, a row not ported
    void (*free_lconv_num)(uint32_t locale);   // 0x4CAD, a row not ported
    void (*free_lc_time)(uint32_t time);       // 0x4D0C, a row not ported
};

// 1.13c D2DDraw+0x03CBB (208 B): ___freetlocinfo(tlocinfo). Free the locale's collate block only
// when it is neither the default (0xEF234) nor NULL and its refcount is zero; free its monetary and
// numeric blocks the same way (each paired with the matching lconv free), then the two blocks
// themselves. Then the ctype block (0x10278) and the 0xFE-byte string block behind +0x44, and the
// time block (0xEF230) with its lc_time free. Finally the record itself.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2ddraw_freetlocinfo(uint8_t* p, const D2DDrawTlocinfoEnv* env) {
    if (*(uint32_t*)(p + 0x3c) != g_clocale_static_ef234 &&
        *(uint32_t*)(p + 0x3c) != 0 &&
        *(int32_t*)(uintptr_t)*(uint32_t*)(p + 0x2c) == 0) {
        uint32_t* q = (uint32_t*)(uintptr_t)*(uint32_t*)(p + 0x34);
        if (q != nullptr && *q == 0 && q != g_lconv_static_1027c) {
            std::free(q);
            env->free_lconv_mon(*(uint32_t*)(p + 0x3c));
        }
        q = (uint32_t*)(uintptr_t)*(uint32_t*)(p + 0x30);
        if (q != nullptr && *q == 0 && q != g_lconv_static_10280) {
            std::free(q);
            env->free_lconv_num(*(uint32_t*)(p + 0x3c));
        }
        std::free((void*)(uintptr_t)*(uint32_t*)(p + 0x2c));
        std::free((void*)(uintptr_t)*(uint32_t*)(p + 0x3c));
    }
    uint32_t* q = (uint32_t*)(uintptr_t)*(uint32_t*)(p + 0x40);
    if (q != g_lconv_static_10278 && q != nullptr && *q == 0) {
        std::free(q);
        std::free((void*)(uintptr_t)(*(uint32_t*)(p + 0x44) - 0xfe));
    }
    uint32_t* t = (uint32_t*)(uintptr_t)*(uint32_t*)(p + 0x50);
    if (t != g_lc_time_static_ef230 && t != nullptr && t[0x2d] == 0) {
        env->free_lc_time((uint32_t)(uintptr_t)t);
        std::free(t);
    }
    std::free(p);
}

// 1.13c D2DDraw+0x03D8B (193 B): ___updatetlocinfo_lk. Take the calling thread's tlocinfo (__getptd
// +0x64); when it differs from the default, decrement its count and those of its five owned
// sub-records, install the default (bumping its count and the counts of its non-null sub-records),
// and free the old record when its count reached zero and it is neither the static 0xE430 record nor
// the default. Returns the thread's (now default) tlocinfo.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2ddraw_updatetlocinfo_lk(const D2DDrawTlocinfoEnv* env) {
    uint8_t* ptd = (uint8_t*)d2_fn_d2ddraw_1bbe();
    uint32_t* old = (uint32_t*)(uintptr_t)*(uint32_t*)(ptd + 0x64);
    uint32_t* def = default_tlocinfo();
    if (old != def) {
        if (old != nullptr) {
            int32_t* q = (int32_t*)(uintptr_t)old[0x0b];
            *old -= 1;
            if (q != nullptr) *q -= 1;
            q = (int32_t*)(uintptr_t)old[0x0d];
            if (q != nullptr) *q -= 1;
            q = (int32_t*)(uintptr_t)old[0x0c];
            if (q != nullptr) *q -= 1;
            q = (int32_t*)(uintptr_t)old[0x10];
            if (q != nullptr) *q -= 1;
            *(int32_t*)(uintptr_t)(old[0x13] + 0xb4) -= 1;
        }
        *(uint32_t*)(ptd + 0x64) = (uint32_t)(uintptr_t)def;
        *def += 1;
        if (def[0x0b] != 0) *(int32_t*)(uintptr_t)def[0x0b] += 1;
        if (def[0x0d] != 0) *(int32_t*)(uintptr_t)def[0x0d] += 1;
        if (def[0x0c] != 0) *(int32_t*)(uintptr_t)def[0x0c] += 1;
        if (def[0x10] != 0) *(int32_t*)(uintptr_t)def[0x10] += 1;
        *(int32_t*)(uintptr_t)(def[0x13] + 0xb4) += 1;
        if (old != nullptr && *old == 0 && old != g_tlocinfo_static_e430)
            d2ddraw_freetlocinfo((uint8_t*)old, env);
    }
    return *(uint32_t*)(ptd + 0x64);
}

// 1.13c D2DDraw+0x03E4C (50 B): ___updatetlocinfo. Take lock 0xC, refresh through
// ___updatetlocinfo_lk, release lock 0xC (the fragment at 0x3E7E) and return the locale pointer.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2ddraw_updatetlocinfo(const D2DDrawTlocinfoEnv* env) {
    d2ddraw_lock(0xC);
    uint32_t locale = d2ddraw_updatetlocinfo_lk(env);
    d2ddraw_sub_3E7E();
    return locale;
}
