#include <cstdint>
#include <cstring>
#include <emscripten.h>

// Host seams for this DLL's CRT heap (retail globals at RVAs 0x1E904/0x1E908).
// Handles and memory remain native-width; allocation words/results remain retail uint32_t.
struct D2GfxMsizeHeap {
    uintptr_t handle;
    uint32_t mode;
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    bool (*find_small_block)(void*);
    uint32_t (*heap_size)(uintptr_t, uint32_t, void*);
};

// Prior art used: D2MOO's D2Gfx is the graphics map, not the bundled CRT.
// Cross-checked the existing D2Game/D2Client CRT heap seams against this 1.13c body.
// 1.13c D2Gfx+0x02796 (__msize): small-block lookup is protected by CRT lock 4;
// a matching block's preceding dword includes eight sentinel bytes and the low allocation bit.
// An unmatched block or another heap mode delegates to HeapSize, preserving 0xFFFFFFFF on failure.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_gfx_msize_2796(void* memory,
                                                         const D2GfxMsizeHeap* heap) {
    if (heap->mode == 3) {
        heap->lock(4);
        const bool small = heap->find_small_block(memory);
        uint32_t size = 0;
        if (small) {
            uint32_t header;
            std::memcpy(&header, static_cast<uint8_t*>(memory) - 4, sizeof header);
            size = header - 9u;
        }
        heap->unlock(4);
        if (small) return size;
    }
    return heap->heap_size(heap->handle, 0, memory);
}
