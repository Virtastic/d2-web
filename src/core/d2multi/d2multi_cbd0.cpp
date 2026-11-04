// 1.13c D2Multi+0xCBD0 (stdcall, ret 8; EDI = name, EBX = password, stack: port, game-name string): join/connect start. The `_exit` branch in front of the
// body tests a constant that is never zero, so it is not modelled. ctx = [39f14]. gv: +0 [39f20], +4 [3a0e0], +8 [3a0dc], +12 [39f88], +16 [19c34] (low u16),
// +20 [39f18] (low u16), +24 the #10054 answer, +28 the #10003 answer (the last two are the probe's script, not globals).
#include <cstring>
#include "d2multi_env.h"

static void cpyz(uint8_t* dst, const char* src) { while ((*dst++ = (uint8_t)*src++) != 0) {} }

extern "C" uint32_t d2multi_cbd0(const D2mEnv& e, uint32_t* gv, uint8_t* ctx, const char* name, const char* pass, uint32_t port, const char* p2) {
    gv[0] = 2;
    d2m_call(e, 0xbe00);
    gv[1] = 0x6f9d8fc0u;   // the stored module address itself (retail D2Multi image base 0x6F9D0000), as the vector holds it
    const uint16_t p16 = (uint16_t)port;
    memcpy(ctx + 0x19, &p16, 2);
    cpyz(ctx + 0x1b, name);
    cpyz(ctx + 0x241, pass);
    if (ctx[0x229] == 0) {
        const uint32_t seven = 7;
        memcpy(ctx + 0x15, &seven, 4);
        cpyz(ctx + 0x33, p2);
        gv[2] = 1;
        d2m_call(e, 0x7d4c, 0, 0, 0, 0, 0, 0);
        d2m_call(e, 0x7ca4);
        return 1;
    }
    const uint32_t r = d2m_call(e, 0x7e2a);
    if (r == 0) {
        gv[0] = 0;
        const uint32_t text = d2m_call(e, 0x7ea8, 0, 0x1413);
        d2m_call(e, 0xc610, 0, 0, 0, 0, 0, text, D2M_MOD(0x8fc0));
        return 0;
    }
    const uint32_t three = 3;
    memcpy(ctx + 0x15, &three, 4);
    uint16_t ax = (uint16_t)gv[4];
    do { ++ax; } while (ax == 0);
    gv[4] = (gv[4] & 0xffff0000u) | ax;
    d2m_call(e, 0x7e54, 0, d2m_ptr(name), d2m_ptr(pass), 0, 0, (r & 0xffff0000u) | ax);
    gv[5] = (gv[5] & 0xffff0000u) | ax;
    d2m_call(e, 0x7c20, 0, gv[3], 1);
    return 1;
}
