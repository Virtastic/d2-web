// 1.13c D2Client+0x880E0: release a sound entry (EDI = entry E; E+0x2c points at link S, E+0x30 = state; S+0 item, S+8 the owning entry).
// Callees: D30C (EAX = S[8], result stored at S[8]+0x41), D336 (ECX = S[0]). A leftover E+0x2c is retail's fatal path (BF92 / BF80 / 3664).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

static uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static void wr32(uint8_t* p, uint32_t o, uint32_t v) { memcpy(p + o, &v, 4); }
static uint8_t* at(uint32_t a) { return (uint8_t*)(uintptr_t)a; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_880e0(const D2cSvc& v, uint8_t* e) {
    if (rd32(e, 0x30) == 1) {
        uint8_t* s = at(rd32(e, 0x2c));
        if (rd32(s, 0) != 0) {
            if (rd32(s, 8) != 0) {
                const uint32_t r = d2c_call(v, 0xd30c, rd32(s, 8), 0, 0, 0, 0);
                wr32(at(rd32(s, 8)), 0x41, r);
            }
            d2c_call(v, 0xd336, 0, rd32(s, 0), 0, 0, 0);
        }
        if (rd32(s, 8) != 0) {
            wr32(at(rd32(s, 8)), 0x30, 2);
            wr32(at(rd32(s, 8)), 0x2c, 0);
            wr32(s, 8, 0);
        }
    }
    if (rd32(e, 0x2c) != 0) {
        const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x93);
        d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xd359f, m);   // DAT_6fb8359f
        d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);              // _exit(-1)
    }
}
