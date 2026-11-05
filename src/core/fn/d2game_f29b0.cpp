// d2game+0xF29B0 probe: r[0] EAX (range in AL), r[1] game, r[2] unit, r[4] result. Blobs lg 0, sc 1.
#include "d2game_s15probe.h"
extern "C" uint32_t d2game_f29b0(uint32_t, uint32_t, uint32_t, const D2S15*);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f29b0(uint32_t* r) {
    uint32_t* L = (uint32_t*)(uintptr_t)r[14];
    HookEnv e(L, (const uint32_t*)(uintptr_t)r[16]);
    D2Seam s{ &e, s15::thunk }; s.bl = s15::blkthunk;
    D2S15 d{ &s, &e, s15::f14 };
    r[4] = d2game_f29b0(r[0], r[1], r[2], &d);
    hk_tag(r, L);
}
