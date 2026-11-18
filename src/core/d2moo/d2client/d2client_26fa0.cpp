// 1.13c D2Client+0x26FA0: pointer-move hit test of the HUD button strip. Stack arg ev (ret 4): ev+0x0C x (u16), +0x0E y (u16),
// +0x18 hit flag, +0x1C cleared on a press edge. The module globals it reads and writes are packed into g[] by the caller:
//  g[0]=+11C3A0 g[1]=+11C3A4 g[2]=+11BBFC (player unit, +0x10 mode, +0x60 link) g[3]=+11C3F8 g[4]=+11C46C (held) g[5]=+DBC48 (screen w)
//  g[6]=+DBC4C (screen h) g[7]=+FADA4 g[8]=+FADFC g[9]=+11C470 g[10]=+11C468.
// Callees: 0x26320 (EAX = ev, returns nonzero while the press is still over the strip) and 0x266C0 (ECX = unit+0x60, EAX = y, EDX = x).
#include <cstring>
#include "d2client_callee.h"
namespace {
constexpr uint32_t kStill = 0x26320, kPress = 0x266c0;
inline void wr(uint8_t* p, uint32_t o, uint32_t x) { std::memcpy(p + o, &x, 4); }
inline uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t x; std::memcpy(&x, p + o, 4); return x; }
}
// 1.13c D2Client+0x26FA0
extern "C" void d2c_26fa0(const D2cSvc& v, uint8_t* ev, uint32_t* g) {
    const uint8_t* unit = (const uint8_t*)(uintptr_t)g[2];
    if (g[0] == 0 || g[1] != 0 || !unit || rd(unit, 0x10) == 0x11) { wr(ev, 0x18, 0); return; }
    const uint32_t link = rd(unit, 0x60);
    uint16_t x16, y16; std::memcpy(&x16, ev + 0xc, 2); std::memcpy(&y16, ev + 0xe, 2);
    const int32_t x = x16, y = y16;
    if (g[3] != 0) return;
    if (g[4] == 0) {
        const int32_t hw = (int32_t)g[5] / 2, h = (int32_t)g[6];
        if (x >= hw + 0x17 && x <= hw + 0x91 && y >= h - 0x27 && y <= h - 10) {
            if (g[7] == 0) {
                g[4] = 1; g[9] = 1;
                d2c_call(v, kPress, y, link, x, 0, 0);
            }
            wr(ev, 0x18, 1); wr(ev, 0x1c, 0);
            return;
        }
        wr(ev, 0x18, 0); g[10] = 0;
        return;
    }
    if (d2c_call(v, kStill, (uint32_t)(uintptr_t)ev, 0, 0, 0, 0) == 0) {
        if (g[8] == 0) g[4] = 0;
        wr(ev, 0x18, 0); g[9] = 0; g[10] = 0;
        return;
    }
    if (g[8] != 0) { wr(ev, 0x18, 1); wr(ev, 0x1c, 0); }
    d2c_call(v, kPress, y, link, x, 0, 0);
}
