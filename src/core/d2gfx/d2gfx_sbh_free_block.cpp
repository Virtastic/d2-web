// src/core/d2gfx/d2gfx_sbh_free_block.cpp -- 1.13c D2Gfx.dll ___sbh_free_block, reimplemented from
// build-wasm/ghidra-d2gfx/decomp/0x43D3.c (alpha lane). Image base 0x6FA80000; every global is named at
// its retail RVA. Callees that are not recovered yet, and every kernel32 import (the wasm module has no
// PE import table), are host seams AFTER the retail arguments, which keep their order (the
// d2mcpclient ___sbh_free_block / d2client_batch_908f.cpp pattern).
#include <cstdint>
#include <emscripten.h>

// --- D2Gfx __sbh_* state (the CRT small-block heap) -------------------------------------------
// File-local: the alloc side of this heap (D2Gfx 0x25E4) is another row and owns its own state.
static uint32_t g_sbh_entry;    // +0x1D8E0 DAT_6fa9d8e0: region record owning the last free
static uint32_t g_sbh_count;    // +0x1D8E4 DAT_6fa9d8e4: region count
static uint32_t g_sbh_regions;  // +0x1D8E8 DAT_6fa9d8e8: region-array base
static uint32_t g_sbh_last;     // +0x1D8F0 DAT_6fa9d8f0: last region handed out
static uint32_t g_sbh_seq;      // +0x1D8F8 DAT_6fa9d8f8: subregion index of the last free
static uint32_t g_sbh_heap;     // +0x1E904 DAT_6fa9e904: Win32 heap handle

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

struct D2GfxSbhEnv {
    void (*virtual_free)(uint32_t addr, uint32_t size, uint32_t type);    // IAT VirtualFree
    uint32_t (*memmove)(uint32_t dst, uint32_t src, uint32_t n);          // 0x061E0 _memmove
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);  // IAT HeapFree
};

// 1.13c D2Gfx+0x043D3
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2gfx_43d3(uint32_t region, uint32_t block,
                                              const D2GfxSbhEnv* env) {
    uint32_t uVar6 = P32(region)[4];                              // heap control block
    uint32_t puVar12 = block - 4;                                 // block header
    uint32_t uVar14 = (uint32_t)(block - P32(region)[3]) >> 0xf;  // subregion index
    uint32_t piVar4 = uVar14 * 0x204 + 0x144 + uVar6;             // bucket descriptor array
    uint32_t local_8 = *P32(puVar12) - 1;
    if ((local_8 & 1) != 0) return 0;                             // already free

    uint32_t puVar10 = local_8 + puVar12;                         // next block
    uint32_t uVar13 = *P32(puVar10);
    uint32_t uVar7 = *P32(block - 8);                             // previous block size word
    if ((uVar13 & 1) == 0) {                                      // next block is free: unlink it
        uint32_t bucket = (uint32_t)(((int32_t)uVar13 >> 4) - 1);
        if (bucket > 0x3f) bucket = 0x3f;
        if (P32(puVar10)[1] == P32(puVar10)[2]) {                 // singleton in its bucket
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
    uint32_t prev_bucket = region;                                // stand-in for param_1
    if ((uVar7 & 1) == 0) {                                       // previous block is free: unlink it
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
    if ((uVar7 & 1) != 0 || prev_bucket != bucket) {              // link the coalesced block
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
    if (*bucketHead != 0) return 0;

    // Last free block in this subregion: decommit the group, and release the region when it empties.
    if (g_sbh_entry != 0) {
        env->virtual_free(g_sbh_seq * 0x8000 + P32(g_sbh_entry)[3], 0x8000, 0x4000);  // MEM_DECOMMIT
        P32(g_sbh_entry)[2] |= 0x80000000u >> (g_sbh_seq & 0x1f);
        *(uint32_t*)(uintptr_t)(P32(g_sbh_entry)[4] + 0xc4 + g_sbh_seq * 4) = 0;
        uint8_t* regcount = P8(P32(g_sbh_entry)[4] + 0x43);
        if (--*regcount == 0) P32(g_sbh_entry)[1] &= 0xfffffffeu;
        if (P32(g_sbh_entry)[2] == 0xFFFFFFFFu) {
            env->virtual_free(P32(g_sbh_entry)[3], 0, 0x8000);                        // MEM_RELEASE
            env->heap_free(g_sbh_heap, 0, P32(g_sbh_entry)[4]);
            env->memmove(g_sbh_entry, g_sbh_entry + 0x14,
                         (uint32_t)(g_sbh_count * 0x14u - g_sbh_entry - 0x14u + g_sbh_regions));
            g_sbh_count -= 1;
            if (g_sbh_entry < region) region -= 20;   // param_1 -= 5 uint words
            g_sbh_last = g_sbh_regions;
        }
    }
    g_sbh_entry = region;
    g_sbh_seq = uVar14;
    return 0;
}
