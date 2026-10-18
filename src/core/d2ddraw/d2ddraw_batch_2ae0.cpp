// src/core/d2ddraw/d2ddraw_batch_2ae0.cpp -- UNMAPPED 1.13c D2DDraw.dll row reimplemented from the
// decompile build-wasm/ghidra-d2ddraw/decomp/0x2AE0.c (alpha lane, no spec/vector yet). Retail base
// 0x6F8C0000; every global is named at its retail RVA.
//
// MSVC 2003 (VC7.1) Release CRT glue: ___sbh_find_block, the small-block heap's
// region lookup. The heap keeps a table of 0x14-byte region headers; each header stores its region
// base as a dword at +0x0C. Walk from the table head up to head + count*0x14 and return the first
// header whose base is within 0x100000 bytes at or below the block (unsigned 32-bit difference, so a
// block below a base wraps high and does not match). No header matches -> 0. The block pointer is
// never dereferenced. The head/count globals are retail module state; nothing else in our port writes
// them, so a zeroed table returns 0 as retail does.
#include <cstdint>
#include <cstring>

static uint32_t g_sbh_region_list;   // +0x104C8  DAT_6f8d04c8: head of the 0x14-byte region headers
static uint32_t g_sbh_region_count;  // +0x104C4  DAT_6f8d04c4: number of region headers

// 1.13c D2DDraw+0x02AE0 (___sbh_find_block, 43 B, leaf; callers 0x1542 _realloc, 0x16F4 __msize,
// 0x1E94 _free): return the region header containing block, or 0.
extern "C"
uint32_t d2ddraw_sbh_find_block(uint32_t block) {
    const uint32_t end = g_sbh_region_list + g_sbh_region_count * 0x14u;
    for (uint32_t h = g_sbh_region_list; h < end; h += 0x14u) {
        uint32_t base;
        std::memcpy(&base, (const void*)(uintptr_t)(h + 0xC), sizeof base);
        if ((uint32_t)(block - base) < 0x100000u) return h;
    }
    return 0;
}
