// VC7.1 realloc, recovered from the cached retail 1.13c function.
// Prior art: D2MOO D2Gfx contains the graphics module, not this linked CRT body;
// d2gfx_heap_alloc.cpp supplies the local heap seam convention. Native pointers
// cross the seams; byte counts and heap selectors retain retail's 32-bit width.
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <emscripten.h>

struct D2GfxReallocState {
    void* heap;                    // +0x1E904
    uint32_t heap_mode;             // +0x1E908
    uint32_t sbh_threshold;         // +0x1D8EC
    uint32_t newmode;               // +0x110CC
    void* (*malloc_)(uint32_t);
    void (*free_)(void*);
    void (*lock)(uint32_t);
    void (*unlock)(uint32_t);
    void* (*sbh_find)(void*);
    int32_t (*sbh_resize)(void* region, void* block, uint32_t size);
    void* (*sbh_alloc)(uint32_t);
    void (*sbh_free)(void* region, void* block);
    void* (*heap_alloc)(void* heap, uint32_t flags, uint32_t size);
    void* (*heap_realloc)(void* heap, uint32_t flags, void* block, uint32_t size);
    int32_t (*callnewh)(uint32_t);
};

static uint32_t sbh_payload_size(const void* block) {
    uint32_t tag;
    std::memcpy(&tag, static_cast<const uint8_t*>(block) - 4, 4);
    return tag - 1u;
}

// 1.13c D2Gfx+0x025E4
// State is owned by the module/caller and persists across calls, including any
// changes made by new-handler callbacks. Failed allocation keeps the old block.
void* d2gfx_realloc_25e4(
    void* block, uint32_t size, D2GfxReallocState* state) {
    if (!block) return state->malloc_(size);
    if (!size) {
        state->free_(block);
        return nullptr;
    }
    if (state->heap_mode == 3) {
        for (;;) {
            if (size > 0xffffffe0u) {
                state->callnewh(size);
                return nullptr;
            }
            void* result = nullptr;
            state->lock(4);
            void* region = state->sbh_find(block);
            if (region) {
                if (size <= state->sbh_threshold) {
                    if (state->sbh_resize(region, block, size)) {
                        result = block;
                    } else {
                        result = state->sbh_alloc(size);
                        if (result) {
                            std::memcpy(result, block, std::min(size, sbh_payload_size(block)));
                            region = state->sbh_find(block);
                            state->sbh_free(region, block);
                        }
                    }
                }
                if (!result) {
                    size = ((size ? size : 1u) + 15u) & 0xfffffff0u;
                    result = state->heap_alloc(state->heap, 0, size);
                    if (result) {
                        std::memcpy(result, block, std::min(size, sbh_payload_size(block)));
                        state->sbh_free(region, block);
                    }
                }
            }
            state->unlock(4); // 0x274C: SEH finally unlocks the small-block heap.
            if (!region) {
                size = ((size ? size : 1u) + 15u) & 0xfffffff0u;
                result = state->heap_realloc(state->heap, 0, block, size);
            }
            if (result) return result;
            if (!state->newmode || !state->callnewh(size)) return nullptr;
        }
    }
    for (;;) {
        if (size > 0xffffffe0u) {
            state->callnewh(size);
            return nullptr;
        }
        if (!size) size = 1;
        void* result = state->heap_realloc(state->heap, 0, block, size);
        if (result) return result;
        if (!state->newmode || !state->callnewh(size)) return nullptr;
    }
}
