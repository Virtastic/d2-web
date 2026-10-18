// src/core/realm/mcp_sbh_free.cpp -- three UNMAPPED 1.13c D2MCPClient.dll rows reimplemented from the
// decompiles in build-wasm/ghidra-d2mcpclient/decomp/0x<RVA>.c (alpha lane). Image base 0x6FA20000;
// every global is named at its retail RVA. Callees that are not recovered yet, and every kernel32
// import (the wasm module has no PE import table), are host seams AFTER the retail arguments, which
// keep their order (the d2client_batch_908f.cpp pattern).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

// --- __sbh_* state (the CRT small-block heap) --------------------------------------------------
// Not static: mcp_sbh_alloc.cpp (0x34FC/0x3887/0x393E/0x3A44/0x3D23) walks the same retail globals.
uint32_t g_sbh_entry;         // +0x1224C  DAT_6fa3224c: region record owning the last free
uint32_t g_sbh_count;         // +0x12250  DAT_6fa32250: region count
uint32_t g_sbh_regions;       // +0x12254  DAT_6fa32254: region-array base
uint32_t g_sbh_max;           // +0x12258  DAT_6fa32258: largest sbh request (heap init argument)
uint32_t g_sbh_last_region;   // +0x1225C  DAT_6fa3225c: last region handed out
uint32_t g_sbh_capacity;      // +0x12260  DAT_6fa32260: region-array capacity
uint32_t g_sbh_seq;           // +0x12264  DAT_6fa32264: subregion index of the last free
uint32_t g_sbh_heap;          // +0x124A0  DAT_6fa324a0: Win32 heap handle

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_sbh_pin(uint32_t entry, uint32_t count, uint32_t regions,
                                          uint32_t last_region, uint32_t seq, uint32_t heap) {
    g_sbh_entry = entry;
    g_sbh_count = count;
    g_sbh_regions = regions;
    g_sbh_last_region = last_region;
    g_sbh_seq = seq;
    g_sbh_heap = heap;
}

struct D2McpSbhEnv {
    void (*virtual_free)(uint32_t addr, uint32_t size, uint32_t type);     // IAT 0x8064 VirtualFree
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);   // heap import
};

// 1.13c D2MCPClient+0x0356F (792 B): ___sbh_free_block(region, block). `local_8 = header - 1`: an odd
// result means the block was already free and the function returns. Otherwise coalesce with the free
// next block (record+0) and the free previous block (header-8), maintaining the two 32-entry bucket
// bitmaps (region[0]/region[4] for buckets 0..0x1F, region[4]+0xC4 for 0x20..0x3F) and the bucket
// counters at heap+4+bucket. Then link the block and, when its subregion empties, decommit it and
// (when the whole region empties) release the region and memmove the tail of the region table down.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_356f(uint32_t region, uint32_t block, const D2McpSbhEnv* env) {
    uint32_t uVar6 = P32(region)[4];                    // heap control block
    uint32_t puVar12 = block - 4;                       // block header
    uint32_t uVar14 = (uint32_t)(block - P32(region)[3]) >> 0xf;   // subregion index
    uint32_t piVar4 = uVar14 * 0x204 + 0x144 + uVar6;   // bucket descriptor array
    uint32_t local_8 = *P32(puVar12) - 1;
    if ((local_8 & 1) != 0) return;

    uint32_t puVar10 = local_8 + puVar12;               // next block
    uint32_t uVar13 = *P32(puVar10);
    uint32_t uVar7 = *P32(block - 8);                   // previous block size word
    if ((uVar13 & 1) == 0) {                            // next block is free: unlink it
        uint32_t bucket = (uint32_t)(((int32_t)uVar13 >> 4) - 1);
        if (bucket > 0x3f) bucket = 0x3f;
        if (P32(puVar10)[1] == P32(puVar10)[2]) {       // singleton in its bucket
            uint8_t* tally = P8(bucket + 4 + uVar6);
            if (bucket < 0x20) {
                uint32_t mask = ~(0x80000000u >> (bucket & 0x1f));
                uint32_t* q = P32(uVar6 + 0x44 + uVar14 * 4);
                *q &= mask;
                if (--*tally == 0) *P32(region) &= mask;
            } else {
                uint32_t mask = ~(0x80000000u >> ((bucket - 0x20) & 0x1f));
                uint32_t* q = P32(uVar6 + 0xc4 + uVar14 * 4);
                *q &= mask;
                if (--*tally == 0) P32(region)[1] &= mask;
            }
        }
        local_8 += uVar13;
        *(uint32_t*)(uintptr_t)(P32(puVar10)[2] + 4) = P32(puVar10)[1];
        *(uint32_t*)(uintptr_t)(P32(puVar10)[1] + 8) = P32(puVar10)[2];
    }

    uint32_t bucket = (uint32_t)(((int32_t)local_8 >> 4) - 1);
    if (bucket > 0x3f) bucket = 0x3f;
    uint32_t prev_bucket = region;                      // stand-in for the decompile's param_1 pointer
    if ((uVar7 & 1) == 0) {                             // previous block is free: unlink it
        puVar12 -= uVar7;
        prev_bucket = (uint32_t)(((int32_t)uVar7 >> 4) - 1);
        if (prev_bucket > 0x3f) prev_bucket = 0x3f;
        local_8 += uVar7;
        bucket = (uint32_t)(((int32_t)local_8 >> 4) - 1);
        if (bucket > 0x3f) bucket = 0x3f;
        if (prev_bucket != bucket) {
            if (P32(puVar12)[1] == P32(puVar12)[2]) {
                uint8_t* tally = P8(prev_bucket + 4 + uVar6);
                if (prev_bucket < 0x20) {
                    uint32_t mask = ~(0x80000000u >> (prev_bucket & 0x1f));
                    uint32_t* q = P32(uVar6 + 0x44 + uVar14 * 4);
                    *q &= mask;
                    if (--*tally == 0) *P32(region) &= mask;
                } else {
                    uint32_t mask = ~(0x80000000u >> ((prev_bucket - 0x20) & 0x1f));
                    uint32_t* q = P32(uVar6 + 0xc4 + uVar14 * 4);
                    *q &= mask;
                    if (--*tally == 0) P32(region)[1] &= mask;
                }
            }
            *(uint32_t*)(uintptr_t)(P32(puVar12)[2] + 4) = P32(puVar12)[1];
            *(uint32_t*)(uintptr_t)(P32(puVar12)[1] + 8) = P32(puVar12)[2];
        }
    }
    if ((uVar7 & 1) != 0 || prev_bucket != bucket) {     // link the coalesced block
        uint32_t piVar1 = piVar4 + bucket * 8;
        uint32_t next = P32(piVar1)[1];
        P32(puVar12)[2] = piVar1;
        P32(puVar12)[1] = next;
        P32(piVar1)[1] = puVar12;
        *(uint32_t*)(uintptr_t)(P32(puVar12)[1] + 8) = puVar12;
        if (P32(puVar12)[1] == P32(puVar12)[2]) {
            uint8_t* tally = P8(bucket + uVar6 + 4);
            uint8_t cVar5 = *tally;
            *tally = (uint8_t)(cVar5 + 1);
            if (bucket < 0x20) {
                uint32_t bit = 0x80000000u >> (bucket & 0x1f);
                if (cVar5 == 0) *P32(region) |= bit;
                uint32_t* q = P32(uVar6 + 0x44 + uVar14 * 4);
                *q |= bit;
            } else {
                uint32_t bit = 0x80000000u >> ((bucket - 0x20) & 0x1f);
                if (cVar5 == 0) P32(region)[1] |= bit;
                uint32_t* q = P32(uVar6 + 0xc4 + uVar14 * 4);
                *q |= bit;
            }
        }
    }
    *P32(puVar12) = local_8;
    *(uint32_t*)(uintptr_t)(local_8 - 4 + puVar12) = local_8;
    uint32_t* bucketHead = P32(piVar4);
    *bucketHead -= 1;
    if (*bucketHead != 0) return;

    if (g_sbh_entry != 0) {
        env->virtual_free(g_sbh_seq * 0x8000 + P32(g_sbh_entry)[3], 0x8000, 0x4000);   // MEM_DECOMMIT
        P32(g_sbh_entry)[2] |= 0x80000000u >> (g_sbh_seq & 0x1f);
        *(uint32_t*)(uintptr_t)(P32(g_sbh_entry)[4] + 0xc4 + g_sbh_seq * 4) = 0;
        uint8_t* regcount = P8(P32(g_sbh_entry)[4] + 0x43);
        if (--*regcount == 0) P32(g_sbh_entry)[1] &= 0xfffffffeu;
        if (P32(g_sbh_entry)[2] == 0xFFFFFFFFu) {
            env->virtual_free(P32(g_sbh_entry)[3], 0, 0x8000);                         // MEM_RELEASE
            env->heap_free(g_sbh_heap, 0, P32(g_sbh_entry)[4]);
            std::memmove(P32(g_sbh_entry), P32(g_sbh_entry + 0x14),
                         (uint32_t)(g_sbh_count * 0x14u - g_sbh_entry - 0x14u + g_sbh_regions));
            g_sbh_count -= 1;
            if (g_sbh_entry < region) region -= 20;   // param_1 -= 5 uint words
            g_sbh_last_region = g_sbh_regions;
        }
    }
    g_sbh_entry = region;
    g_sbh_seq = uVar14;
}

struct D2McpReportFailureEnv {
    void (*sub_5603)(uint32_t code);    // 0x05603 CRT security-failure reporter, noreturn
    void (*exit_process)(uint32_t code);  // IAT ExitProcess
};

// 1.13c D2MCPClient+0x04794 (41 B): report_failure -- `push 1; call sub_5603; push 3; call ExitProcess`.
// Both callees are unrecovered/imported, so they are the whole rule; the function does not return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_mcp_4794(const D2McpReportFailureEnv* env) {
    env->sub_5603(1);
    env->exit_process(3);
}

struct D2McpSbhAllocEnv {
    uint32_t (*new_region)(void);             // 0x3887 ___sbh_alloc_new_region
    int32_t  (*new_group)(uint32_t region);   // 0x393E ___sbh_alloc_new_group
};

// 1.13c D2MCPClient+0x03D23 (764 B, callers 0x1C25 calloc, 0x2841 __heap_alloc, 0x29EA _realloc):
// ___sbh_alloc_block(size). `uVar7 = (size + 0x17) & ~0xF` is the rounded block size (header included),
// `b = ((size + 0x17) >> 4) - 1` the wanted bucket. Each region's two 32-bit bucket bitmaps (region[0]
// for buckets 0..0x1F, region[1] for 0x20..0x3F, bit `0x80000000 >> (bucket & 0x1F)`) select the buckets
// `>= b`: `uVar14 = 0xFFFFFFFF >> b` and `local_c = 0xFFFFFFFF >> (b - 0x20)` for the high word when
// b >= 0x20. Walk the region list from the last-used region to its end, then from the head up to the
// last-used region. On a miss, take the first region whose [2] word is non-zero (a committed group
// slot); if that scan also misses, allocate a region (0x3887), then a group (0x393E), which stores its
// index through region[4] and returns -1 on failure. Pick the group (region[4][0], else the first group
// whose bitmaps intersect the masks), take the first free block of the first acceptable bucket, unlink
// it, re-bin the remainder into its own bucket (refreshing the bitmaps and the per-group bucket tally
// bytes at heap+4+bucket), split it, and return the payload after the uVar7-byte header.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_mcp_sbh_alloc_block(uint32_t size, const D2McpSbhAllocEnv* env) {
    uint32_t uVar7 = (size + 0x17u) & 0xfffffff0u;
    int32_t iVar8 = (int32_t)((size + 0x17u) >> 4) - 1;
    uint32_t puVar9 = g_sbh_regions + g_sbh_count * 0x14u;
    uint8_t bVar6 = (uint8_t)iVar8;
    uint32_t param = g_sbh_last_region;
    uint32_t uVar14, local_c;
    if (iVar8 < 0x20) {
        uVar14 = 0xffffffffu >> (bVar6 & 0x1f);
        local_c = 0xffffffffu;
    } else {
        uVar14 = 0;
        local_c = 0xffffffffu >> ((bVar6 - 0x20) & 0x1f);
    }
    while (param < puVar9 && (P32(param)[1] & local_c) == 0 && (P32(param)[0] & uVar14) == 0)
        param += 0x14;
    uint32_t puVar12 = g_sbh_regions;
    if (param == puVar9) {
        while (puVar12 < g_sbh_last_region && (P32(puVar12)[1] & local_c) == 0 &&
               (P32(puVar12)[0] & uVar14) == 0)
            puVar12 += 0x14;
        param = puVar12;
        if (puVar12 == g_sbh_last_region) {
            while (puVar12 < puVar9 && P32(puVar12)[2] == 0) puVar12 += 0x14;
            uint32_t puVar13 = g_sbh_regions;
            param = puVar12;
            if (puVar12 == puVar9) {
                while (puVar13 < g_sbh_last_region && P32(puVar13)[2] == 0) puVar13 += 0x14;
                param = puVar13;
                if (puVar13 == g_sbh_last_region) {
                    param = env->new_region();
                    if (param == 0) return 0;
                }
            }
            iVar8 = env->new_group(param);
            *P32(P32(param)[4]) = (uint32_t)iVar8;
            if (*P32(P32(param)[4]) == 0xffffffffu) return 0;
        }
    }
    uint32_t piVar5 = P32(param)[4];                       // heap control block
    int32_t local_8 = (int32_t)P32(piVar5)[0];             // group index [0, 8)
    if (local_8 == -1 ||
        ((P32(piVar5)[local_8 + 0x31] & local_c) == 0 && (P32(piVar5)[local_8 + 0x11] & uVar14) == 0)) {
        local_8 = 0;
        uint32_t puVar9b = piVar5 + 0x44;
        if ((P32(piVar5)[0x31] & local_c) == 0 && (P32(puVar9b)[0] & uVar14) == 0) {
            do {
                uint32_t pv = puVar9b + 0x84;              // group n+1's high bitmap
                local_8 += 1;
                puVar9b += 4;                              // group n+1's low bitmap
                if ((P32(pv)[0] & local_c) != 0 || (P32(puVar9b)[0] & uVar14) != 0) break;
            } while (true);
        }
    }
    uint32_t piVar3 = piVar5 + local_8 * 0x204 + 0x144;    // this group's 0x40 bucket descriptors
    int32_t bucket = 0;
    uint32_t mask = P32(piVar5)[local_8 + 0x11] & uVar14;
    if (mask == 0) {
        mask = P32(piVar5)[local_8 + 0x31] & local_c;
        bucket = 0x20;
    }
    while ((int32_t)mask >= 0) {                           // shift until the first set bit reaches bit 31
        bucket += 1;
        mask <<= 1;
    }
    uint32_t piVar11 = P32(piVar3)[bucket * 2 + 1];        // first free block of that bucket
    int32_t iVar10 = (int32_t)P32(piVar11)[0] - (int32_t)uVar7;   // remainder size
    int32_t newbucket = (iVar10 >> 4) - 1;
    if (newbucket > 0x3f) newbucket = 0x3f;
    g_sbh_last_region = param;
    if (newbucket != bucket) {
        if (P32(piVar11)[1] == P32(piVar11)[2]) {          // singleton: drop its bit
            uint8_t* tally = P8(piVar5 + bucket + 4);
            if (bucket < 0x20) {
                uint32_t m = ~(0x80000000u >> (bucket & 0x1f));
                P32(piVar5)[local_8 + 0x11] &= m;
                if (--*tally == 0) P32(param)[0] &= m;
            } else {
                uint32_t m = ~(0x80000000u >> ((bucket - 0x20) & 0x1f));
                P32(piVar5)[local_8 + 0x31] &= m;
                if (--*tally == 0) P32(param)[1] &= m;
            }
        }
        *(uint32_t*)(uintptr_t)(P32(piVar11)[2] + 4) = P32(piVar11)[1];
        *(uint32_t*)(uintptr_t)(P32(piVar11)[1] + 8) = P32(piVar11)[2];
        if (iVar10 != 0) {                                 // link the remainder into its own bucket
            uint32_t piVar1 = piVar3 + newbucket * 8;
            uint32_t next = P32(piVar1)[1];
            P32(piVar11)[2] = piVar1;
            P32(piVar11)[1] = next;
            P32(piVar1)[1] = piVar11;
            *(uint32_t*)(uintptr_t)(P32(piVar11)[1] + 8) = piVar11;
            if (P32(piVar11)[1] == P32(piVar11)[2]) {      // singleton: set its bit
                uint8_t* tally = P8(piVar5 + newbucket + 4);
                uint8_t old = *tally;
                *tally = (uint8_t)(old + 1);
                if (newbucket < 0x20) {
                    uint32_t bit = 0x80000000u >> (newbucket & 0x1f);
                    if (old == 0) P32(param)[0] |= bit;
                    P32(piVar5)[local_8 + 0x11] |= bit;
                } else {
                    uint32_t bit = 0x80000000u >> ((newbucket - 0x20) & 0x1f);
                    if (old == 0) P32(param)[1] |= bit;
                    P32(piVar5)[local_8 + 0x31] |= bit;
                }
            }
        }
    }
    if (iVar10 != 0) {                                     // the free block keeps its head and footer
        P32(piVar11)[0] = (uint32_t)iVar10;
        *(uint32_t*)(uintptr_t)((uint32_t)((int32_t)piVar11 + iVar10) - 4) = (uint32_t)iVar10;
    }
    piVar11 = (uint32_t)((int32_t)piVar11 + iVar10);       // the allocated block
    P32(piVar11)[0] = uVar7 + 1;
    *(uint32_t*)(uintptr_t)(piVar11 + uVar7 - 4) = uVar7 + 1;
    int32_t was = (int32_t)P32(piVar3)[0];
    P32(piVar3)[0] = (uint32_t)(was + 1);
    if (was == 0 && param == g_sbh_entry && local_8 == (int32_t)g_sbh_seq) g_sbh_entry = 0;
    P32(piVar5)[0] = (uint32_t)local_8;
    return piVar11 + 4;
}
