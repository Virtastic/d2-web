// 1.13c D2Client+0x28050: pointer-down hit test of the bottom HUD strip (left and right orbs, the two text toggles, the two
// small buttons). Stack arg ev (ret 4): ev+0x0C x (u16), +0x0E y (u16); writes ev+0x18 (handled) and +0x1C (0).
// Module globals packed into g[] by the caller:
//  g[0]=+11C3A4 g[1]=+11BBFC (player unit, +0x10 mode) g[2]=+DBC48 (screen w) g[3]=+DBC4C (screen h) g[4]=+11C478 g[5]=+11C458
//  g[6]=+F5388 g[7]=+11C45C g[8]=+11C4B0 (HP text) g[9]=+11C4B4 (MP text) g[10]=+11C4A4 g[11]=+11C4AC.
// Callees: 0x26320 (EAX = ev), 0xBE84 (stdcall ev, handled notify), 0xBEF6 (stdcall key, name, 0, value: saves a setting),
// 0x88A70 (EBX = 4, four zero stack words). 0x26560 and 0x265A0 are pure range tests of (ECX x, ESI y), inlined.
#include <cstring>
#include "d2client_callee.h"
namespace {
constexpr uint32_t kStill = 0x26320, kNotify = 0xbe84, kSave = 0xbef6, kBeep = 0x88a70;
inline void wr(uint8_t* p, uint32_t o, uint32_t x) { std::memcpy(p + o, &x, 4); }
inline uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t x; std::memcpy(&x, p + o, 4); return x; }
}
// 1.13c D2Client+0x28050
extern "C" void d2c_28050(const D2cSvc& v, uint8_t* ev, uint32_t* g) {
    const uint8_t* unit = (const uint8_t*)(uintptr_t)g[1];
    if (g[0] != 0 || !unit || rd(unit, 0x10) == 0x11) { wr(ev, 0x18, 0); return; }
    uint16_t x16, y16; std::memcpy(&x16, ev + 0xc, 2); std::memcpy(&y16, ev + 0xe, 2);
    const int32_t x = x16, y = y16, w = (int32_t)g[2], h = (int32_t)g[3];
    const uint32_t evp = (uint32_t)(uintptr_t)ev;
    const auto done = [&](bool flag6) {
        if (flag6) g[6] = 1;
        wr(ev, 0x18, 1); wr(ev, 0x1c, 0);
        d2c_call(v, kNotify, 0, 0, 0, 0, 0, evp);
    };
    if (d2c_call(v, kStill, evp, 0, 0, 0, 0)) { g[4] = 1; wr(ev, 0x18, 1); wr(ev, 0x1c, 0); d2c_call(v, kNotify, 0, 0, 0, 0, 0, evp); return; }
    if (y <= h - 0x30) { wr(ev, 0x18, 0); return; }
    if (x >= 0x75 && x <= 0xa5 && y <= h) { g[4] = 1; g[5] = 1; done(true); return; }
    if (x >= w - 0xa5 && x <= w - 0x75 && y <= h) { g[4] = 1; g[7] = 1; done(true); return; }
    if (x >= 0x1e && x <= 0x6e && y >= h - 0x4b && y <= h - 0xf) {
        g[8] = g[8] == 0;
        d2c_call(v, kSave, 0, 0, 0, 0, 0, 0xB20D3588u, 0xB20D6B68u, 0, g[8]);
        done(true); return;
    }
    if (x >= w - 0x6f && x <= w - 0x1f && y >= h - 0x4b && y <= h - 0xf) {
        g[9] = g[9] == 0;
        d2c_call(v, kSave, 0, 0, 0, 0, 0, 0xB20D3588u, 0xB20D6B58u, 0, g[9]);
        done(true); return;
    }
    const int32_t hw = w / 2;                              // retail: cdq / sub / sar, truncation toward zero
    if (x >= hw - 8 && x <= hw + 5 && y >= h - 0x27 && y <= h - 0xd) { g[10] = 1; g[4] = 1; d2c_call(v, kBeep, 0, 0, 0, 4, 0, 0, 0, 0, 0); }
    else if (x >= hw - 0x91 && x <= hw - 0x80 && y >= h - 0x1c && y <= h - 8) { g[11] = 1; g[4] = 1; d2c_call(v, kBeep, 0, 0, 0, 4, 0, 0, 0, 0, 0); }
    done(true);
}
