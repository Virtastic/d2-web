// Prior art used: D2MOO's D2Gfx has no CRT malloc implementation. The same VC7.1 retry
// wrapper is modelled in src/core/realm/mcp_crt_batch_b58.cpp; checked against the cached
// D2Gfx 1.13c decompile. Allocator and new-handler calls are seams after retail arguments.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Gfx+0x02887 (__nh_malloc, 44 B): reject oversized requests before either call.
// Otherwise retry __heap_alloc (0x280C) only while nh_flag and __callnewh (0x4345)
// permit it. Size stays retail-width; an allocation remains a full native pointer.
extern "C" EMSCRIPTEN_KEEPALIVE void* d2gfx_nh_malloc(
    uint32_t size, int32_t nh_flag, void* (*heap_alloc)(uint32_t),
    int32_t (*call_newh)(uint32_t)) {
    if (size >= 0xffffffe1u) return nullptr;
    for (;;) {
        void* allocation = heap_alloc(size);
        if (allocation != nullptr) return allocation;
        if (nh_flag == 0 || call_newh(size) == 0) return nullptr;
    }
}
