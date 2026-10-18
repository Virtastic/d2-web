// src/core/realm/bnclient_batch_4a3c.cpp -- one UNMAPPED 1.13c BNClient.dll alpha-lane row (Stage A:
// implementation + marker only, no spec, no vector), transcribed from
// build-wasm/ghidra-bnclient/decomp/0x4A3C.c (image base 0x6FF20000).
//
// Row: ___sbh_free_block 0x4A3C, MSVC 2003 (VC7.1) Release CRT small-block heap. Byte-for-byte the same
// rule as D2MCPClient 0x356F (src/core/realm/mcp_sbh_free.cpp); the BN module's own globals are passed
// in (DAT_6FF40BC4 entry, BC8 count, BCC region array, BD4 last region, BDC subregion, BE0 heap) so the
// two DLLs do not share heap state. VirtualFree/HeapFree are seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct BnSbhFreeEnv {
    void     (*virtual_free)(uint32_t addr, uint32_t size, uint32_t type);   // IAT VirtualFree
    int32_t  (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);    // kernel heap import
    uint32_t* entry;        // DAT_6FF40BC4: region record owning the last free
    uint32_t* count;        // DAT_6FF40BC8: region count
    uint32_t* regions;      // DAT_6FF40BCC: region-array base
    uint32_t* last_region;  // DAT_6FF40BD4: last region handed out
    uint32_t* seq;          // DAT_6FF40BDC: subregion index of the last free
    uint32_t* heap;         // DAT_6FF40BE0: Win32 heap handle
};

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }
static inline uint8_t*  P8(uint32_t a)  { return (uint8_t*)(uintptr_t)a; }

// 1.13c BNClient+0x04A3C (792 B, callers 0x2275/0x3CF4): ___sbh_free_block(region, block). A header word
// minus one that is odd means the block is already free: return. Otherwise coalesce with the free next
// block (record+0) and free previous block (header-8), maintaining the two 32-entry bucket bitmaps
// (region[0] and region[4]+0xC4) and the bucket tally bytes at heap+4+bucket. Link the coalesced block,
// stamp its size at both ends, and if its subregion empties decommit it and, when the whole region
// empties, release the region and memmove the region table tail down.
extern "C"
EMSCRIPTEN_KEEPALIVE void bn_sbh_free_block(uint32_t region, uint32_t block, const BnSbhFreeEnv* env) {
    uint32_t* const g_entry = env->entry;
    uint32_t* const g_count = env->count;
    uint32_t* const g_regions = env->regions;
    uint32_t* const g_last_region = env->last_region;
    uint32_t* const g_seq = env->seq;
    uint32_t* const g_heap = env->heap;

    uint32_t uVar6 = P32(region)[4];                       // heap control block
    uint32_t puVar12 = block - 4;                          // block header
    uint32_t uVar14 = (uint32_t)(block - P32(region)[3]) >> 0xf;   // subregion index
    uint32_t piVar4 = uVar14 * 0x204 + 0x144 + uVar6;      // bucket descriptor array
    uint32_t local_8 = *P32(puVar12) - 1;
    if ((local_8 & 1) != 0) return;

    uint32_t puVar10 = local_8 + puVar12;                  // next block
    uint32_t uVar13 = *P32(puVar10);
    uint32_t uVar7 = *P32(block - 8);                      // previous block size word
    if ((uVar13 & 1) == 0) {                               // next block free: unlink it
        uint32_t bucket = (uint32_t)(((int32_t)uVar13 >> 4) - 1);
        if (bucket > 0x3f) bucket = 0x3f;
        if (P32(puVar10)[1] == P32(puVar10)[2]) {          // singleton in its bucket
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
    uint32_t prev_bucket = region;                        // stand-in for the decompile's param_1 pointer
    if ((uVar7 & 1) == 0) {                               // previous block free: unlink it
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
    if ((uVar7 & 1) != 0 || prev_bucket != bucket) {       // link the coalesced block
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

    if (*g_entry != 0) {
        env->virtual_free(*g_seq * 0x8000 + P32(*g_entry)[3], 0x8000, 0x4000);   // MEM_DECOMMIT
        P32(*g_entry)[2] |= 0x80000000u >> (*g_seq & 0x1f);
        *(uint32_t*)(uintptr_t)(P32(*g_entry)[4] + 0xc4 + *g_seq * 4) = 0;
        uint8_t* regcount = P8(P32(*g_entry)[4] + 0x43);
        if (--*regcount == 0) P32(*g_entry)[1] &= 0xfffffffeu;
        if (P32(*g_entry)[2] == 0xFFFFFFFFu) {
            env->virtual_free(P32(*g_entry)[3], 0, 0x8000);                       // MEM_RELEASE
            env->heap_free(*g_heap, 0, P32(*g_entry)[4]);
            std::memmove(P32(*g_entry), P32(*g_entry + 0x14),
                         (uint32_t)(*g_count * 0x14u - *g_entry - 0x14u + *g_regions));
            *g_count -= 1;
            if (*g_entry < region) region -= 20;   // param_1 -= 5 uint words
            *g_last_region = *g_regions;
        }
    }
    *g_entry = region;
    *g_seq = uVar14;
}
