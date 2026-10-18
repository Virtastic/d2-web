// 1.13c D2Gfx+0x0280C (111 B, caller 0x2887 ___nh_malloc): __heap_alloc, the VC7.1 CRT allocation
// leaf. Small-block heap first when the heap is in mode 3 (+0x1E908) and the request fits the sbh
// threshold (+0x1D8EC): __lock(4), ___sbh_alloc_block(0x4B87), _unlock(4) (0x287E), and the block is
// returned when non-null. Otherwise HeapAlloc(+0x1E904, 0, size), with a zero request raised to one
// byte and the size rounded up to 16 unless the heap is in mode 1.
// Prior art: build-wasm/ghidra-d2gfx/decomp/0x280C.c (this row); the same VC7.1 leaf modelled for
// D2Client 0x7CB8 (src/core/d2client/d2client_batch_77a1.cpp) and D2MCPClient 0x2841.
// Retail globals and callees are host seams after the retail argument (the d2gfx_sbh_* pattern).
#include <cstdint>
#include <emscripten.h>

struct D2GfxHeapAllocEnv {
    uint32_t heap_mode;        // +0x1E908 DAT_6fa9e908
    uint32_t sbh_threshold;    // +0x1D8EC DAT_6fa9d8ec
    uint32_t heap;             // +0x1E904 DAT_6fa9e904: Win32 heap handle
    void     (*lock4)(void);                                  // 0x1A71 __lock(4)
    void     (*unlock4)(void);                                // 0x287E _unlock(4)
    uint32_t (*sbh_alloc_block)(uint32_t size);               // 0x4B87 ___sbh_alloc_block
    uint32_t (*heap_alloc)(uint32_t heap, uint32_t flags, uint32_t size);  // IAT HeapAlloc
};

// 1.13c D2Gfx+0x0280C
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2gfx_280c(uint32_t size, const D2GfxHeapAllocEnv* env) {
    if (env->heap_mode == 3 && size <= env->sbh_threshold) {
        env->lock4();
        uint32_t p = env->sbh_alloc_block(size);
        env->unlock4();
        if (p != 0) return p;
    }
    uint32_t bytes = size == 0 ? 1 : size;
    if (env->heap_mode != 1) bytes = (bytes + 0x0f) & 0xfffffff0u;
    return env->heap_alloc(env->heap, 0, bytes);
}
