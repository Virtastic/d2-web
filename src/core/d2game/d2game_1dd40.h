#pragma once
#include <cstdint>

// 1.13c D2Game+0x1DD40 (pet / minion owner-follow tick): ECX = pGame, EDX = pUnit, stack = pParam (ret 4; the result is residue). Written from the 1.13c disassembly as a
// rule over E (see d2game_19080.h); every callee is a hook. P points at a pointer whose target has +0x14 = the follow threshold word.
// owner = (unit type 1, unit+0x14 then +0x28 then +0x28 non-null) ? 0xE03A0(EAX = b[+0x30], ECX = b[+0x28], EDX = b[+0x2c]) : 0. No owner, or owner type word nonzero: 0x9D230
// (ECX = game, EDX = unit, *P, 5 when class 0x10F else 6), 0xF22C0 (EAX = game, ESI = unit, 0xA), ends. Else: A68E(unit, 0xC) != 0 runs A3CA(unit, 0xC, 0); mode = unit+0x10 (2 or 0xF
// ends). t = [*P + 0x14]; 0x10 < t < 0x14 gives l14 = t, hi = 2t, l24 = t >> 1, else l14 = 0x10, hi = 0x18, l24 = 5. r = 0xB0A20(EBX = unit, owner); pet move 0x1D3C0 (EAX = unit, game,
// owner, a, b, c, d): r > 0x64 (3,0,0,0); r > hi (1,1,0x3C,0); r > l14 and owner mode (owner+0x10) 2 or 6 (0,0,0,0), 3 (0,1,0x3C,0). Mode != 1: each type-2 state node of unit+0xDC
// gets 0x8E6F0, then 0xF22C0 (.., 5), ends. Mode 1: k = class not in {0x152, 0x230, 0x231}; 0xAC6A(A478(unit), x, y, 0xAD00(unit), 0x40) != 0 (l20 = 1): roll 0x1450(0x80) < (k ? 0xC : 6)
// runs 0xF1B10 (AL = 5, game, unit) and ends. Then A808(A478(unit)) == 0, 0xB1BC0(EAX = unit, ECX = game, EBX = &dummy, &dist) != 0 and dist < 0x19 runs 0x1C8A0 (EAX = unit, game,
// class, owner, result, unit + 0x20, P) and ends. l20 != 0 runs 0xF1B10 (AL = 5) and ends. AD66(A478(owner), x, y) != AD66(A478(unit), x, y) gives pet move (0,1,0x3C,0). 0xB0A20 <= 1 gives
// pet move (5,0,0,l24 - 1). roll 0x1180(100, seed) < 5 runs 0xF2870 (AL = l14, ECX = owner, game, unit) else 0xF22C0 (.., 5).
namespace g1dd40 {
template <class E> void Think(E& e, uint32_t game, uint32_t unit, uint32_t P)
{
    // 1.13c D2Game+0x1DD40
    const uint32_t S = unit + 0x20;
    uint32_t cls = 0, owner = 0;
    auto idle = [&](uint32_t a) { e.h(0xF22C0, game, 0, 0, 0, unit, a); };
    auto d230 = [&](uint32_t k) { e.h(0x9D230, 0, game, unit, 0, 0, e.rd32(P), k); };
    auto pm = [&](uint32_t a, uint32_t b, uint32_t c, uint32_t d) { e.h(0x1D3C0, unit, 0, 0, 0, 0, game, owner, a, b, c, d); };
    if (unit) {
        cls = e.rd32(unit + 4);
        if (e.rd32(unit) == 1) {
            const uint32_t a = e.rd32(unit + 0x14);
            if (a) {
                const uint32_t b = e.rd32(a + 0x28);
                if (b) { const uint32_t c = e.rd32(b + 0x28); if (c) owner = e.h(0xE03A0, e.rd32(b + 0x30), c, e.rd32(b + 0x2c)); }
            }
        }
    }
    if (!owner || e.rd32(owner)) { d230(unit && cls == 0x10F ? 5 : 6); idle(0xA); return; }
    if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const uint32_t mode = e.rd32(unit + 0x10);
    if (mode == 2 || mode == 0xF) return;
    const int32_t t = (int32_t)e.rd32(e.rd32(P) + 0x14);
    uint32_t l14 = 0x10, hi = 0x18, l24 = 5;
    if (t > 0x10 && t < 0x14) { l14 = (uint32_t)t; hi = (uint32_t)t * 2; l24 = (uint32_t)t >> 1; }
    const uint32_t r = e.h(0xB0A20, 0, 0, 0, unit, 0, owner), om = e.rd32(owner + 0x10);
    if (r > 0x64) { pm(3, 0, 0, 0); return; }
    if (r > hi) { pm(1, 1, 0x3C, 0); return; }
    if (r > l14) {
        if (om == 2 || om == 6) { pm(0, 0, 0, 0); return; }
        if (om == 3) { pm(0, 1, 0x3C, 0); return; }
    }
    uint32_t l20 = 0;
    if (mode != 1) {
        for (uint32_t n = e.rd32(unit + 0xDC); n;) { const uint32_t nx = e.rd32(n + 0x24); if (e.rd8(n) == 2) e.h(0x8E6F0, n); n = nx; }
        idle(5); return;
    }
    const bool k = !(cls == 0x152 || (cls > 0x22F && cls <= 0x231));
    {
        const uint32_t id = e.h(0xAD00, 0, 0, 0, 0, 0, unit);
        const uint32_t y = e.h(0x1130, 0, unit), x = e.h(0x1100, 0, unit);
        const uint32_t an = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        if (e.h(0xAC6A, 0, 0, 0, 0, 0, an, x, y, id, 0x40)) {
            l20 = 1;
            const int32_t roll = (int32_t)e.h(0x1450, 0, 0, 0, 0, 0, 0x80);
            if (roll < (k ? 0xC : 6)) { e.h(0xF1B10, ((uint32_t)roll & ~0xFFu) | 5, 0, 0, 0, 0, game, unit); return; }
        }
    }
    {
        const uint32_t an = e.h(0xA478, 0, 0, 0, 0, 0, unit);
        if (!e.h(0xA808, 0, 0, 0, 0, 0, an)) {
            uint32_t dummy = 0, dist = 0;
            const uint32_t br = e.h(0xB1BC0, unit, game, 0, e.stk(&dummy), 0, e.stk(&dist));
            if (br && dist < 0x19) {
                uint32_t w[2] = { S, P };
                e.h(0x1C8A0, unit, 0, 0, 0, 0, game, cls, owner, br, S, P);
                e.x(0x1C8A0, w, 2);
                return;
            }
        }
    }
    if (l20) { e.h(0xF1B10, 5, 0, 0, 0, 0, game, unit); return; }
    uint32_t ab[2];
    for (int i = 0; i < 2; i++) {
        const uint32_t who = i ? unit : owner;
        const uint32_t y = e.h(0x1130, 0, who), x = e.h(0x1100, 0, who);
        const uint32_t an = e.h(0xA478, 0, 0, 0, 0, 0, who);
        ab[i] = e.h(0xAD66, 0, 0, 0, 0, 0, an, x, y);
    }
    if (ab[0] != ab[1]) { pm(0, 1, 0x3C, 0); return; }
    const uint32_t r2 = e.h(0xB0A20, 0, 0, 0, unit, 0, owner);
    if (r2 <= 1) { pm(5, 0, 0, (l24 & ~0xFFu) | ((l24 - 1) & 0xFF)); return; }
    const int32_t roll = (int32_t)e.h(0x1180, 100, S);
    if (roll >= 5) { idle(5); return; }
    e.h(0xF2870, ((uint32_t)roll & ~0xFFu) | (l14 & 0xFF), owner, 0, 0, 0, game, unit);
}
}
