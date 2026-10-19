#include <cstddef>
#include <cstdint>
#include <cstring>

#include <emscripten/emscripten.h>

struct D2GameMsizeHeap {
    uintptr_t handle;
    uint32_t mode;
    void (*lock)(int32_t);
    void (*unlock)(int32_t);
    uintptr_t (*find_small_block)(void*);
    uint32_t (*heap_size)(uintptr_t, uint32_t, void*);
};

// 1.13c D2Game+0x05E5B (__msize): in small-block mode, query under CRT lock 4 and
// use the allocation header only if the pointer belongs to a small-block region.
// Otherwise ask the process heap. The callbacks represent this DLL's CRT and Win32 heap.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_game_msize_5e5b(void* memory,
                                                              const D2GameMsizeHeap* heap) {
    if (heap->mode == 3) {
        heap->lock(4);
        const uintptr_t region = heap->find_small_block(memory);
        uint32_t size = 0;
        if (region != 0) {
            uint32_t header;
            std::memcpy(&header, static_cast<uint8_t*>(memory) - 4, sizeof header);
            size = header - 9u;
        }
        heap->unlock(4);
        if (region != 0) return size;
    }
    return heap->heap_size(heap->handle, 0, memory);
}
