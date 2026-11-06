// 1.13c D2Client+0x8B0C0 (ESI = obj, stack p1, ret 4; DAT_6fbcc414 = g): obj[3] or obj[4] nonzero: if g != 3 call 7A9F0(eax = obj, stack p1); clear bit 0 of obj[0].
// A null obj is retail's fatal path (BF92 / BF80 / 3664).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

static uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
static void wr32(uint8_t* p, uint32_t o, uint32_t v) { memcpy(p + o, &v, 4); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_8b0c0(const D2cSvc& v, uint8_t* obj, uint32_t p1, uint32_t g_cc414) {
    if (!obj) {
        const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0x1dd);
        d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xd359f, m);
        d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    if (rd32(obj, 0xc) != 0 || rd32(obj, 0x10) != 0) {
        if (g_cc414 != 3) d2c_call(v, 0x7a9f0, (uint32_t)(uintptr_t)obj, 0, 0, 0, 0, p1);
        wr32(obj, 0, rd32(obj, 0) & 0xfffffffeu);
    }
}
