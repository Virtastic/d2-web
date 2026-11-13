// superseded port of D2Multi 0xC610 (stdcall, ret 8): BE00, rebuild the four control slots, load the sprite, record the callback; returns 1.
// gv: +0 [39f88], +4 [39154], +8 [39150], +12 [39158], +16 [3915c], +20 [3a0d8] (out), +24 [3a0e0] (out).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_c610(const D2mEnv& e, uint32_t* gv, uint32_t word, uint32_t callback) {
    d2m_call(e, 0xbe00);
    if (gv[0] != 0) d2m_call(e, 0x7c20, 0, gv[0], 0);
    d2m_call(e, 0x7c1a, 0, gv[1], 0);
    d2m_call(e, 0x7c1a, 0, gv[2], 0);
    d2m_call(e, 0x7c1a, 0, gv[3], 0);
    d2m_call(e, 0x7c1a, 0, gv[4], 0);
    uint32_t h = d2m_call(e, 0x7caa, 0, 0, 0, 0, 0, D2M_MOD(0x1a2a0));
    gv[5] = h;
    d2m_call(e, 0x7c56, 0, h, word);
    gv[6] = callback;
    return 1;
}
