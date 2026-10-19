// Prior art: D2MOO does not carry the linked VC7.1 CRT heap teardown. Cross-checked
// the existing D2Gfx 0x33C8 reading against D2Game's cached 1.13c decompile.
#include "d2game_heap_init.h"
#include <emscripten.h>

// Relocated host representation of each retail 0x14-byte region record: the
// three metadata words precede the reservation (+0xC) and control block (+0x10).
// These are native pointers, not serialized x86 addresses or packed structures.
struct D2GameHeapRegion {
    uint32_t metadata[3];
    void* reservation;
    void* control;
};

struct D2GameHeapTermEnv {
    int32_t (*virtual_free)(void*, uint32_t, uint32_t);
    int32_t (*heap_free)(void*, uint32_t, void*);
    int32_t* region_count;             // DAT_6FD39FB8, signed
    D2GameHeapRegion* regions;         // DAT_6FD39FBC, relocated region array
};

// 1.13c D2Game+0x0679B
// Small-block mode releases each reservation before its control block, then the
// region array. Every mode destroys the heap. Retail ignores API failures and
// leaves the module's handles/count untouched; the count is reread per iteration.
extern "C" EMSCRIPTEN_KEEPALIVE void d2game_heap_term_679b(
    const D2GameHeapInitEnv* heap, const D2GameHeapTermEnv* env) {
    if (heap->heap_mode == 3) {
        D2GameHeapRegion* region = env->regions;
        for (int32_t i = 0; i < *env->region_count; ++i, ++region) {
            env->virtual_free(region->reservation, 0x100000, 0x4000);
            env->virtual_free(region->reservation, 0, 0x8000);
            env->heap_free(heap->heap, 0, region->control);
        }
        env->heap_free(heap->heap, 0, env->regions);
    }
    heap->heap_destroy(heap->heap);
}
