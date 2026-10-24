// 1.13c D2Game+0x3CB00 probe: eax=pOrigin r[0], ecx=slot r[1] (chain 0xF829C -> var -> dt), edx=missile id r[2], stack game r[3], owner r[5], skill r[6], level r[7], step r[8]; r[9]=log.
// A model: the request is a stack local (only its address reaches the log), the unit x/y thunks 0xA454/0xA3B8 are hooked.
#include <emscripten.h>
#include "hookenv.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_3cb00(uint32_t* r) {
    static const uint32_t sc[] = { 0xA454, 0x111, 0xA3B8, 0x222, 0x6F930, 1, 0, 0 };   // the descriptor's hook values
    HookEnv e((uint32_t*)(uintptr_t)r[9], sc);
    const uint32_t dt = HookEnv::rd32(HookEnv::rd32(HookEnv::rd32(r[1])));
    const int32_t id = (int32_t)r[2];
    if (id < 0 || id >= (int32_t)HookEnv::rd32(dt + 0xB6C) || id * 0x1A4 + HookEnv::rd32(dt + 0xB64) == 0) { r[4] = 0; return; }
    const uint32_t ty = HookEnv::rd32(r[0]), unit = HookEnv::rd32(r[0] + 0x2C);
    const bool direct = ty == 2 || (ty > 3 && ty < 6);
    if (!direct && unit) { e.h(0xA454, 0, 0, 0, 0, 0, unit); e.h(0xA3B8, 0, 0, 0, 0, 0, unit); }
    uint32_t q[23] = {};
    for (int32_t i = 0; i < 8; i += (int32_t)r[8]) e.h(0x6F930, 0, r[3], e.stk(q));
    r[4] = 1;
}
