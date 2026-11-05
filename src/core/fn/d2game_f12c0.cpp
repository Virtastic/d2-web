// d2game+0xF12C0 probe: r[0] = the unit (stack word 0), r[4] result. Blobs lg 0, sc 1.
#include "d2game_s15probe.h"
extern "C" uint32_t d2game_f12c0(uint32_t, const D2Seam*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f12c0(uint32_t* r) {
    uint32_t* L = (uint32_t*)(uintptr_t)r[14];
    HookEnv e(L, (const uint32_t*)(uintptr_t)r[16]);
    D2Seam s{ &e, s15::thunk }; s.bl = s15::blkthunk;
    r[4] = d2game_f12c0(r[0], &s);
    hk_tag(r, L);
}
