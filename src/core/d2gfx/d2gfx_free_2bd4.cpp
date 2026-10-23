#include <cstdint>
#include <emscripten.h>

// 1.13c D2Gfx+0x02BD4 (104 B, _free). Retail globals and callees are seams: heap_type = +0x1E908
// (__active_heap), heap = +0x1E904 (_crtheap); lock4/unlock4 = __lock(4) 0x1A71 / 0x2C27;
// find = ___sbh_find_block 0x43A8; sbh_free = ___sbh_free_block 0x43D3; heap_free = HeapFree.
struct D2GfxFreeEnv {
    uint32_t heap_type;
    void (*lock4)();
    void (*unlock4)();
    void* (*find)(void* p);
    void (*sbh_free)(void* region, void* p);
    void (*heap_free)(void* p);
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_free(void* p, const D2GfxFreeEnv* e) {
    if (p == nullptr) return;
    if (e->heap_type == 3) {
        e->lock4();
        void* region = e->find(p);
        if (region != nullptr) e->sbh_free(region, p);
        e->unlock4();                       // runs whether or not the block was found
        if (region != nullptr) return;
    }
    e->heap_free(p);
}
