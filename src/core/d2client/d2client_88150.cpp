// 1.13c D2Client+0x88150: walk the chain at E+0x28 of {item, next} nodes (EDI = entry E), D258(ecx = item+0x78, edx = E[8]) per node,
// then tail-jump D25E(ecx = &E[0x28]). A zero item is retail's fatal path (BF92 / BF80 / 3664).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_callee.h"

static uint32_t rd32(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2c_88150(const D2cSvc& v, uint8_t* e) {
    const uint32_t head = (uint32_t)(uintptr_t)e + 0x28;
    for (uint32_t n = rd32(e, 0x28); n != 0; n = rd32((const uint8_t*)(uintptr_t)n, 4)) {
        const uint32_t item = rd32((const uint8_t*)(uintptr_t)n, 0);
        if (item == 0) {
            const uint32_t m = d2c_call(v, 0xbf92, 0, 0, 0, 0, 0, 0xc2c);
            d2c_call(v, 0xbf80, 0, 0, 0, 0, 0, 0xB2000000u | 0xd359f, m);
            d2c_call(v, 0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
            return;
        }
        d2c_call(v, 0xd258, 0, item + 0x78, rd32(e, 8), 0, 0);
    }
    d2c_call(v, 0xd25e, 0, head, 0, 0, 0);
}
