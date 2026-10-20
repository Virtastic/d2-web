// Prior art used: CRT RE map and D2Gfx's VC7.1 SBH initializer.
// D2MOO has no CRT allocator body. Rechecked against the cached D2Game 1.13c
// decompile; preserve publication on allocation failure and native pointer width.
#include "d2game_sbh_state.h"
#include <emscripten.h>

// 1.13c D2Game+0x08BD5 (___sbh_heap_init): own the region-table state, not an FN
// snapshot. The HeapAlloc seam receives the same heap, flags and byte count.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_sbh_heap_init_8bd5(
    uint32_t maximum, void* heap, D2GameSbhState* state, D2GameSbhHeapAlloc heap_alloc) {
    state->regions = heap_alloc(heap, 0, 0x140);
    if (!state->regions) return 0;
    state->pending_region = nullptr;
    state->region_count = 0;
    state->cursor = state->regions;
    state->maximum_request = maximum;
    state->capacity = 16;
    return 1;
}
