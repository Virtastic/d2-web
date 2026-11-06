// 1.13c D2Client+0x6B5A0, written from the disassembly. EBX = unit, stdcall 2 (x, y), returns 0 or 1.
// Only a type-4 unit draws. kind = 0: when 0xC220(unit) == 4, 0xC1C6(unit, 0xE, 0) = v picks kind 1 (100 <= v < 500), 2 (< 5000) or 3.
// A zeroed 0x48-byte frame {+5 = 1, +0x40 = kind} goes to 0x6E2C0(EAX=-1; unit, &frame, &local) and then 0xC0D0(&frame, 0, 1); either
// zero returns 0. With cel = frame+0x3C: 0x6D280(ESI=cel; x, y + 0xC0E2(cel), 0) zero returns 0. Then t = 0xCC04(0, unit, &y slot, 1);
// m = 0xC1A8(unit, 0x400000, 0xAC2, "D2Client.cpp") ? 1 : 5; 0xD096(&frame, x, y + 0xC0E2(cel), -1, m, t); and the overlay
// 0x6AEE0(unit, 0xFF, 0, x + 0xC0D6(cel) / 2, y + 0xC0E2(cel) / 2, 0). Returns 1.
#include <cstring>
#include "cl_env.h"
extern "C" uint32_t d2c_6b5a0(uint32_t unit, uint32_t x, uint32_t y, const cl::Env& e) {
    const auto P = [](const void* p) { return (uint32_t)(uintptr_t)p; };
    if (!unit || e.R(unit) != 4) return 0;
    uint32_t kind = 0;
    if (e.H(0xc220, unit) == 4) {
        const uint32_t v = e.H(0xc1c6, unit, 0xe, 0);
        if (v >= 0x64) kind = v < 0x1f4 ? 1 : v < 0x1388 ? 2 : 3;
    }
    uint32_t fr[18] = {}, snap[18], loc = 0, ys = y;
    fr[0x34 / 4] = 0;
    fr[0x40 / 4] = kind;
    fr[0] = 0;
    ((uint8_t*)fr)[5] = 1;
    memcpy(snap, fr, sizeof fr);
    const uint32_t ok = e.H(0x6e2c0, unit, P(fr), P(&loc), 0, 0, 0xffffffffu);
    if (e.blk) e.blk(e.ctx, 0x6e2c0, P(snap), 18);
    if (!ok) return 0;
    memcpy(snap, fr, sizeof fr);
    const uint32_t r0 = e.H(0xc0d0, P(fr), 0, 1);
    if (e.blk) e.blk(e.ctx, 0xc0d0, P(snap), 18);
    if (!r0) return 0;
    const uint32_t cel = fr[0x3c / 4];
    if (!e.H(0x6d280, x, e.H(0xc0e2, cel) + y, 0, 0, 0, 0, 0, 0, 0, cel)) return 0;
    const uint32_t t = e.H(0xcc04, 0, unit, P(&ys), 1);
    const uint32_t m = e.H(0xc1a8, unit, 0x400000, 0xac2, 0xB20D4FA4u) ? 1 : 5;
    const uint32_t h = e.H(0xc0e2, cel);
    memcpy(snap, fr, sizeof fr);
    e.Xs(t, 0);
    e.H(0xd096, P(fr), x, h + y, 0xffffffffu, m);
    if (e.blk) e.blk(e.ctx, 0xd096, P(snap), 18);
    const uint32_t hy = e.H(0xc0e2, cel) >> 1;
    const uint32_t wx = e.H(0xc0d6, cel) >> 1;
    uint32_t a[6] = { unit, 0xff, 0, wx + x, hy + y, 0 };
    e.H(0x6aee0, a[0], a[1], a[2], a[3], a[4]);
    if (e.blk) e.blk(e.ctx, 0x6aee0, P(a), 6);
    return 1;
}
