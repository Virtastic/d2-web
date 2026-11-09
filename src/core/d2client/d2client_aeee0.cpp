// 1.13c D2Client+0xAEEE0: object/missile-spawn packet handler (ECX = packet). Checked against the disassembly: the 0x17-dword request is
// zeroed first, [1] = the current unit global, [4] = word @+5, [5] = dword @+7, [6] = dword @+0xB, [0xC] = byte @+0x1E; the unit comes
// back from D2Client+0xB30F0 (ECX = request).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_aeee0.h"
namespace {
template <class T> T rd(const uint8_t* p, uint32_t o) { T v; std::memcpy(&v, p + o, sizeof v); return v; }
}
extern "C" {
// 1.13c D2Client+0xAEEE0
EMSCRIPTEN_KEEPALIVE void d2c_aeee0(const D2cAEEE0Env* e, const uint8_t* p) {
    const uint32_t a = rd<uint32_t>(p, 0xf), b = rd<uint32_t>(p, 0x13), c = rd<uint32_t>(p, 0x1a);
    const uint16_t w17 = rd<uint16_t>(p, 0x17);
    uint32_t l[0x17] = {0};
    l[1] = e->curUnit;
    l[5] = rd<uint32_t>(p, 7);
    l[6] = rd<uint32_t>(p, 0xb);
    l[0xc] = rd<uint8_t>(p, 0x1e);
    l[4] = rd<uint16_t>(p, 5);
    l[0] = 0x40000001;
    if (a && b) { l[0] = 0x40000021; l[7] = a; l[8] = b; }
    const uintptr_t u = e->fB30F0(l);
    if (!u) return;
    e->o10069(u, rd<uint8_t>(p, 0x19), c);
    const uint8_t f = rd<uint8_t>(p, 0x1f);
    if (f) e->o10887(u, 0x148, f, 0);
    e->o10970(u, w17);
    uint32_t v = e->f1380(u + 0x20) << 8;
    std::memcpy((void*)(u + 0x44), &v, 4);
}
}
