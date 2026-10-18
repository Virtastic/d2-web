// src/core/d2gfx/d2gfx_batch_2c45.cpp -- 1.13c D2Gfx.dll calloc (203 B), reimplemented from
// build-wasm/ghidra-d2gfx/decomp/0x2C45.c (alpha lane). Image base 0x6FA80000; every global is named at
// its retail RVA. Callees that are not recovered in this file, and the kernel32 import (the wasm module has
// no PE import table), are host seams AFTER the retail arguments, which keep their order (the
// d2gfx_sbh_free_block / d2gfx_getptd pattern). The injected __SEH_prolog/__SEH_epilog (0x1B2C/0x1B67)
// frame the __lock(4) ... __unlock(4) bracket and have no body here.
#include <cstdint>
#include <emscripten.h>

// --- D2Gfx CRT heap state, all zero-initialised -----------------------------------------------
static uint32_t g_heap;           // +0x1E904 DAT_6fa9e904: Win32 process heap handle
static uint32_t g_heap_type;      // +0x1E908 DAT_6fa9e908: heap selector (3 = the small-block heap)
static uint32_t g_sbh_threshold;  // +0x1D8EC DAT_6fa9d8ec: largest request the small-block heap takes
static uint32_t g_newmode;        // +0x110CC DAT_6fa910cc: new-handler mode; 0 = no retry

struct D2GfxCallocEnv {
    void     (*lock)(uint32_t locknum);                              // 0x01A71 __lock
    void     (*unlock)(uint32_t locknum);                            // 0x02D0B: __unlock(4) leaf (0x019BC)
    uint32_t (*sbh_alloc_block)(uint32_t size);                      // 0x04B87 ___sbh_alloc_block
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t bytes);  // IAT HeapAlloc
    void     (*memset_)(uint32_t dst, uint32_t value, uint32_t n);   // 0x05890 _memset
    int32_t  (*callnewh)(uint32_t size);                             // 0x04345 __callnewh
};

// 1.13c D2Gfx+0x02C45 (203 B): `void * __cdecl calloc(size_t count, size_t size)`.
// Reject an overflowing product (count != 0 && size > 0xFFFFFFE0 / count) and answer NULL; a zero product
// asks for one byte. While the request stays below 0xFFFFFFE1: when the heap selector is 3 the request is
// rounded up to 16 bytes first, and a request at or below the small-block threshold takes the small-block
// path under __lock(4)/__unlock(4), zeroing exactly the requested bytes on success and returning them.
// Otherwise (or on small-block failure) HeapAlloc(heap, HEAP_ZERO_MEMORY, rounded) is tried. A non-null
// block is returned; on failure no new-handler mode ends the call in NULL, and __callnewh(size) decides
// whether the whole allocation is retried (rounded size carried forward) or the call answers NULL.
// 1.13c D2Gfx+0x02C45
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2gfx_2c45(uint32_t count, uint32_t size,
                                               const D2GfxCallocEnv* env) {
    if (count != 0 && size > 0xffffffe0u / count) return 0;
    uint32_t request = count * size;
    uint32_t bytes = request != 0 ? request : 1;
    for (;;) {
        uint32_t block = 0;
        if (bytes < 0xffffffe1u) {
            if (g_heap_type == 3) {
                bytes = (bytes + 0xfu) & 0xfffffff0u;
                if (request <= g_sbh_threshold) {
                    env->lock(4);
                    block = env->sbh_alloc_block(request);
                    env->unlock(4);
                    if (block != 0) {
                        env->memset_(block, 0, request);
                        return block;
                    }
                }
            }
            block = env->heap_alloc(g_heap, 8, bytes);   // HEAP_ZERO_MEMORY
        }
        if (block != 0) return block;
        if (g_newmode == 0) return 0;
        if (env->callnewh(bytes) == 0) return 0;
    }
}
