// src/core/d2gfx/d2gfx_heap_term_33c8.cpp -- 1.13c D2Gfx.dll __heap_term (127 B), reimplemented from
// build-wasm/ghidra-d2gfx/decomp/0x33C8.c (alpha lane). Image base 0x6FA80000; every global is named at
// its retail RVA. The three kernel32 calls (VirtualFree, HeapFree, HeapDestroy) are host seams AFTER the
// retail arguments, which keep their order (the d2gfx_sbh_alloc_new_region pattern).
#include <cstdint>
#include <emscripten.h>

// --- D2Gfx CRT heap state (file-local: each alpha-lane file owns its copy until the heap is wired) ---
static uint32_t g_heap;       // +0x1E904 DAT_6fa9e904: Win32 process heap handle
static uint32_t g_heap_mode;  // +0x1E908 DAT_6fa9e908: heap selector (3 = the small-block heap)
static int32_t  g_sbh_count;  // +0x1D8E4 DAT_6fa9d8e4: small-block region count
static uint32_t g_sbh_regions; // +0x1D8E8 DAT_6fa9d8e8: small-block region-array base

struct D2GfxHeapTermEnv {
    int32_t (*virtual_free)(uint32_t addr, uint32_t size, uint32_t free_type); // IAT VirtualFree
    int32_t (*heap_free)(uint32_t heap, uint32_t flags, uint32_t block);       // IAT HeapFree
    int32_t (*heap_destroy)(uint32_t heap);                                    // IAT HeapDestroy
};

// 127 B. __heap_term(void). Nothing is released unless the small-block heap was selected (mode 3): then
// each of the count region records (0x14 bytes each; record[3] at +0xC is its 1 MiB committed base and
// record[4] at +0x10 its control block) has the base decommitted (MEM_DECOMMIT 0x4000, 0x100000 bytes)
// and released (MEM_RELEASE 0x8000, size 0), and the control block HeapFree'd; finally the region array
// is HeapFree'd. The process heap is destroyed unconditionally afterwards, even when the selector is not 3.
// 1.13c D2Gfx+0x033C8
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2gfx_33c8(const D2GfxHeapTermEnv* env) {
    if (g_heap_mode == 3) {
        for (int32_t i = 0; i < g_sbh_count; ++i) {
            uint32_t region = g_sbh_regions + (uint32_t)i * 0x14u;
            uint32_t base = *(const uint32_t*)(uintptr_t)(region + 0xc);
            uint32_t control = *(const uint32_t*)(uintptr_t)(region + 0x10);
            env->virtual_free(base, 0x100000, 0x4000);  // MEM_DECOMMIT
            env->virtual_free(base, 0, 0x8000);         // MEM_RELEASE
            env->heap_free(g_heap, 0, control);
        }
        env->heap_free(g_heap, 0, g_sbh_regions);
    }
    env->heap_destroy(g_heap);
}
