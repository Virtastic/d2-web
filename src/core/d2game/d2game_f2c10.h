// 1.13c D2Game+0xF2C10: EAX = a (reference), EBX = b (mover), stack game, d, run (ret 0xC). Positions: unit types 2 and 4..5 read path+0xC / +0x10 (path = [unit+0x2C]), other types
// ask thunk 0xA454 (x) / 0xA3B8 (y) for a non-null path (0 for none); read order b.x, b.y, a.x, a.y. sx / sy = -1 if a > b on that axis (unsigned), 1 if a < b, else 0; nx = bx + dd * sx,
// ny = by + dd * sy with dd = d & 0xFF. dd > 5: b's AI data ([[b+0x14]+0x2C] for a type-1 b, else 0) byte +0x20 = min(dd, 0x4D). Then 0xF2540 (ECX and EDX clear; stack 0, 0xF, nx, ny, 1, run ? 4 : 0).
#pragma once
#include <cstdint>
namespace gf2c10 {
template <class E> void Run(E& e, uint32_t a, uint32_t b, uint32_t game, uint32_t d, uint32_t run)
{
    // 1.13c D2Game+0xF2C10
    auto fld = [&](uint32_t u, uint32_t off, uint32_t thunk) -> uint32_t {
        const int32_t t = (int32_t)e.rd32(u);
        const uint32_t p = e.rd32(u + 0x2C);
        if (t == 2 || (t > 3 && t <= 5)) return e.rd32(p + off);
        return p ? e.h(thunk, 0, 0, 0, 0, 0, p) : 0u;
    };
    const uint32_t bx = fld(b, 0xC, 0xA454);
    const uint32_t by = fld(b, 0x10, 0xA3B8);
    const uint32_t ax = fld(a, 0xC, 0xA454);
    const uint32_t ay = fld(a, 0x10, 0xA3B8);
    const int32_t sx = ax > bx ? -1 : (ax < bx ? 1 : 0), sy = ay > by ? -1 : (ay < by ? 1 : 0);
    const uint32_t dd = d & 0xFFu;
    const uint32_t nx = bx + dd * (uint32_t)sx, ny = by + dd * (uint32_t)sy;
    if (dd > 5) {
        uint32_t ai = 0;
        if (e.rd32(b) == 1 && e.rd32(b + 0x14)) ai = e.rd32(e.rd32(b + 0x14) + 0x2C);
        e.wr8(ai + 0x20, dd >= 0x4E ? 0x4D : dd);
    }
    const uint32_t w[6] = { 0, 0xF, nx, ny, 1, run ? 4u : 0u };
    e.h(0xF2540, 0, game, b, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5]);
}
}
