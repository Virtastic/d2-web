// 1.13c D2Game+0x0674A (__heap_init, 81 B, caller 0x35BB). Same VC7.1 body as D2Client 0x08C7F.
// Callees: ___heap_select (0x6730, d2_fn_d2game_6730) and ___sbh_heap_init (0x8BD5,
// d2game_sbh_heap_init_8bd5) are the real src/ definitions; HeapCreate/HeapDestroy are kernel32
// imports, host seams after the retail arguments.
#include "d2game_sbh_state.h"
#include <emscripten.h>

extern "C" int32_t d2_fn_d2game_6730(uint32_t osPlatform, uint32_t winMajor);

struct D2GameHeapInitEnv {
    void*   (*heap_create)(uint32_t options, uint32_t initial_commit, uint32_t maximum_size);
    int32_t (*heap_destroy)(void* heap);
    D2GameSbhHeapAlloc heap_alloc;   // HeapAlloc seam for 0x8BD5
    D2GameSbhState* sbh;             // 0x8BD5 region-table state
    uint32_t os_platform;            // 0x6FD2FD24
    uint32_t win_major;              // 0x6FD2FD30
    void*    heap;                   // 0x6FD3A20C DAT_6fd3a20c
    uint32_t mode;                   // 0x6FD3A210 DAT_6fd3a210
};

extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2game_heap_init_674a(int32_t mtflag, D2GameHeapInitEnv* e) {
    e->heap = e->heap_create(mtflag == 0 ? 1u : 0u, 0x1000, 0);
    if (!e->heap) return 0;
    e->mode = d2_fn_d2game_6730(e->os_platform, e->win_major);
    if (e->mode == 3 && d2game_sbh_heap_init_8bd5(0x3f8, e->heap, e->sbh, e->heap_alloc) == 0) {
        e->heap_destroy(e->heap);
        return 0;
    }
    return 1;
}
