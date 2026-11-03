// 1.13c D2Game+0x43FA0, transcribed from the 1.13c
// disassembly. Convention: ECX = pGame, EDX = unit, two stack words (index idx into the 0x23C-byte table at dt+0xB98, count dt+0xBA0; p), ret 8.
// Returns 0 on a bad index / row field (row+0x48 as int16 against dt+0xB6C, row+0x80 as int16 against dt+0xC4) or a null 0xA3F4(unit, idx) handle,
// else 1. Skill gate: 0xA68E(unit, row+0x80) != 0 and 0xACB2(handle) == 0 takes the first branch (0xAD12, 0x1EF0, then the 0xA0F40 creator,
// 0x42510 / 0x3EDE0 / 0xE20A0 / 0x7A190 on its result, 0xAC94 with the result's +0xC word or -1); otherwise a 9-dword request block
// {unit, unit, idx, p, 0xAC9A(unit, row+0x60, idx, p), -1, 0, row+0x80, 0} goes to 0xA00B0 and a non-null result gets two 0xA6E2 sets (0x15E idx, 0x15F p).
// Both end with 0xACDC(handle, 0).
#pragma once
#include <cstdint>

namespace r43fa0 {
template <class E> uint32_t Do(E& e, uint32_t game, uint32_t unit, uint32_t dt, uint32_t idx, uint32_t p, uint32_t rec, uint32_t recTag)
{
    if ((int32_t)idx < 0) return 0;
    if ((int32_t)idx >= (int32_t)e.rd32(dt + 0xBA0)) return 0;
    const uint32_t row = idx * 0x23C + e.rd32(dt + 0xB98);
    if (!row) return 0;
    const int32_t c48 = e.rd16s(row + 0x48);
    if (c48 < 0 || c48 >= (int32_t)e.rd32(dt + 0xB6C)) return 0;
    const int32_t sk = e.rd16s(row + 0x80);
    if (sk < 0 || sk >= (int32_t)e.rd32(dt + 0xC4)) return 0;
    const uint32_t hd = e.h(0xA3F4, 0, unit, idx);
    if (!hd) return 0;
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)sk) && !e.h(0xACB2, 0, 0, 0, 0, 0, hd)) {
        const uint32_t ad = e.h(0xAD12, 0, 0, 0, 0, 0, hd);
        const uint32_t ef = e.h(0x1EF0, idx);
        const uint32_t a0 = e.h(0xA0F40, 3, ad, ef, 0, 0, game, unit, 0, 0);
        if (!a0) {
            e.h(0xAC94, 0, 0, 0, 0, 0, hd, 0xFFFFFFFFu);
        } else {
            const uint32_t it = e.h(0x42510, unit, 0, 0, 0, 0, game, (uint32_t)c48, idx, p);
            if (it) {
                e.h(0x3EDE0, 0, 0, 0, 0, 0, game, recTag, 1);
                e.h(0xE20A0, 0, game, it);
                e.h(0x7A190, 0, 0, 0, 0, 0, idx, p, 0, 0, 0);
            }
            e.h(0xAC94, 0, 0, 0, 0, 0, hd, e.rd32(rec + 0xC));
        }
    } else {
        uint32_t b[9] = { unit, unit, idx, p, 0, 0xFFFFFFFFu, 0, (uint32_t)sk, 0 };
        b[4] = e.h(0xAC9A, 0, 0, 0, 0, 0, unit, e.rd32(row + 0x60), idx, p);
        const uint32_t nu = e.h(0xA00B0, e.stk(b));
        e.blk(0xA00B0, b, 9);
        if (nu) {
            e.h(0xA6E2, 0, 0, 0, 0, 0, nu, 0x15E, idx, 0);
            e.h(0xA6E2, 0, 0, 0, 0, 0, nu, 0x15F, p, 0);
        }
    }
    e.h(0xACDC, 0, 0, 0, 0, 0, hd, 0);
    return 1;
}
}
