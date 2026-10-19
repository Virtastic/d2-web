// src/core/d2gfx/d2gfx_sbh_heap_init.cpp -- 1.13c D2Gfx.dll ___sbh_heap_init, reimplemented from
// build-wasm/ghidra-d2gfx/decomp/0x4360.c (alpha lane). Image base 0x6FA80000; every global is named at
// its retail RVA. The kernel32 import is a host seam AFTER the retail argument, which keeps its order
// (the d2gfx_sbh_alloc_new_region pattern).
#include <cstdint>
#include <emscripten.h>

// --- D2Gfx __sbh_* state (the CRT small-block heap) -------------------------------------------
// File-local: the init side of this heap owns its own state (the d2gfx_sbh_free_block pattern).
static uint32_t g_sbh_entry;    // +0x1D8E0 DAT_6fa9d8e0: region record owning the last free
static uint32_t g_sbh_count;    // +0x1D8E4 DAT_6fa9d8e4: region count
static uint32_t g_sbh_regions;  // +0x1D8E8 DAT_6fa9d8e8: region-array base
static uint32_t g_sbh_max;      // +0x1D8EC DAT_6fa9d8ec: maximum request size
static uint32_t g_sbh_last;     // +0x1D8F0 DAT_6fa9d8f0: last region handed out
static uint32_t g_sbh_cap;      // +0x1D8F4 DAT_6fa9d8f4: region-array capacity (records)
static uint32_t g_sbh_heap;     // +0x1E904 DAT_6fa9e904: Win32 heap handle

struct D2GfxSbhHeapInitEnv {
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t size); // IAT HeapAlloc
};

// 1.13c D2Gfx+0x04360 (72 B): ___sbh_heap_init(dwMaximum). Take the 0x140-byte region table from the
// Win32 heap (flags 0) and store it as the region-array base. On failure return 0 with state untouched;
// otherwise reset the sbh globals: no region owning a free, none allocated, the last-region cursor at
// the table head, the caller's maximum request size, and room for 0x10 regions. Returns 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2gfx_4360(uint32_t maximum, const D2GfxSbhHeapInitEnv* env) {
    uint32_t regions = env->heap_alloc(g_sbh_heap, 0, 0x140);
    g_sbh_regions = regions;
    if (regions == 0) return 0;
    g_sbh_entry = 0;
    g_sbh_count = 0;
    g_sbh_last = regions;
    g_sbh_max = maximum;
    g_sbh_cap = 0x10;
    return 1;
}
