// src/core/d2gfx/d2gfx_sbh_alloc_new_region.cpp -- 1.13c D2Gfx.dll ___sbh_alloc_new_region,
// reimplemented from build-wasm/ghidra-d2gfx/decomp/0x46EB.c (alpha lane). Image base 0x6FA80000;
// every global is named at its retail RVA. Every kernel32 import is a host seam AFTER the retail
// arguments, which keep their order (the d2gfx_sbh_alloc_block pattern).
#include <cstdint>
#include <emscripten.h>

// --- D2Gfx __sbh_* state (the CRT small-block heap) -------------------------------------------
// File-local: the alloc side of this heap owns its own state (the d2gfx_sbh_free_block pattern).
static uint32_t g_sbh_count;    // +0x1D8E4 DAT_6fa9d8e4: region count
static uint32_t g_sbh_regions;  // +0x1D8E8 DAT_6fa9d8e8: region-array base
static uint32_t g_sbh_cap;      // +0x1D8F4 DAT_6fa9d8f4: region-array capacity (records)
static uint32_t g_sbh_heap;     // +0x1E904 DAT_6fa9e904: Win32 heap handle

static inline uint32_t* P32(uint32_t a) { return (uint32_t*)(uintptr_t)a; }

struct D2GfxSbhNewRegionEnv {
    uint32_t (*heap_realloc)(uint32_t heap, uint32_t flags, uint32_t block, uint32_t size); // IAT HeapReAlloc
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t size);                   // IAT HeapAlloc
    uint32_t (*virtual_alloc)(uint32_t addr, uint32_t size, uint32_t type, uint32_t protect); // IAT VirtualAlloc
    int32_t  (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);                   // IAT HeapFree
};

// 183 B. ___sbh_alloc_new_region(void). Grow the region array by 16 records
// when it is full ((cap*5+0x50)*4 bytes, cap += 0x10), take the next free record (base + count*0x14),
// HeapAlloc a 0x41C4-byte control block for it (flags 8), commit a 1 MiB VirtualAlloc'd base
// (0, 0x100000, MEM_COMMIT 0x2000, PAGE_READWRITE 4), then reset region state: subregion count -1,
// the two bucket bitmaps 0, the control block's current-group field -1, count++. A failed VirtualAlloc
// frees the control block; either failure returns 0 without advancing count. Returns the record.
// 1.13c D2Gfx+0x046EB
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2gfx_46eb(const D2GfxSbhNewRegionEnv* env) {
    if (g_sbh_count == g_sbh_cap) {
        uint32_t p = env->heap_realloc(g_sbh_heap, 0, g_sbh_regions, (g_sbh_cap * 5u + 0x50u) * 4u);
        if (p == 0) return 0;
        g_sbh_cap += 0x10u;
        g_sbh_regions = p;
    }
    uint32_t region = g_sbh_regions + g_sbh_count * 0x14u;
    uint32_t control = env->heap_alloc(g_sbh_heap, 8, 0x41c4);
    P32(region)[4] = control;
    if (control != 0) {
        uint32_t base = env->virtual_alloc(0, 0x100000, 0x2000, 4);
        P32(region)[3] = base;
        if (base != 0) {
            P32(region)[2] = 0xffffffffu;
            P32(region)[0] = 0;
            P32(region)[1] = 0;
            g_sbh_count++;
            P32(control)[0] = 0xffffffffu;
            return region;
        }
        env->heap_free(g_sbh_heap, 0, control);
    }
    return 0;
}
