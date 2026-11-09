// 1.13c D2Client+0xC65B0, written from the disassembly. EAX = unit, stack (target), ret 4. No unit or no path [target+0x2C] returns. a = 0xCB68(unit), b = 0xCB6E(unit),
// esi = 0xC496(path) >> 8 (zero returns). d = 0xC4D8(target, a, b), at least 1. q = (d << 16) / (esi << 12), minus 1 when above 1. The 2-row table at D2Client+0xD5F54
// ({limit, mul, add} words) picks row 0 when d < limit0, else row 1; cost = max(mul * d + add, 500). 0xC7B00(EAX = target); [[target+0x54]+0x30] (when both exist)
// gets +0x1C = 0, +0x20 = 0, +0x24 = -cost; then 0xC7C80(EAX = target, ECX = q, stack 0).
#include "cl_mb2.h"
// 1.13c D2Client+0xC65B0
extern "C" uint32_t d2c_c65b0(uint32_t u, uint32_t tg, const clm::Env& e) {
    if (!u) return 0;
    const uint32_t path = e.R(tg + 0x2c);
    if (!path) return 0;
    const uint32_t a = e.H(0xcb68, 0, 0, 0, 0, 0, u);
    const uint32_t b = e.H(0xcb6e, 0, 0, 0, 0, 0, u);
    const int32_t sz = (int32_t)e.H(0xc496, 0, 0, 0, 0, 0, path) >> 8;
    if (!sz) return 0;
    int32_t d = (int32_t)e.H(0xc4d8, 0, 0, 0, 0, 0, tg, a, b);
    if (d < 1) d = 1;
    const int32_t den = (int32_t)((uint32_t)sz << 12);
    int32_t q = (int32_t)((uint32_t)d << 16) / den;
    if (q > 1) --q;
    uint32_t row = 0, addr = 0xd5f54;
    do {
        if (d < (int32_t)e.G(addr)) break;
        addr += 12; ++row;
    } while (addr < 0xd5f60);
    int32_t cost = (int32_t)(e.G(0xd5f58 + 12 * row) * (uint32_t)d + e.G(0xd5f5c + 12 * row));
    if (cost < 0x1f4) cost = 0x1f4;
    e.H(0xc7b00, tg);
    const uint32_t o = e.R(tg + 0x54);
    if (o) {
        const uint32_t o2 = e.R(o + 0x30);
        if (o2) { e.W(o2 + 0x1c, 0); e.W(o2 + 0x20, 0); e.W(o2 + 0x24, (uint32_t)-cost); }
    }
    e.H(0xc7c80, tg, (uint32_t)q, 0, 0, 0, 0);
    return 0;
}
