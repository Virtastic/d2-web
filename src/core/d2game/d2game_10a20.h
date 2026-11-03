// 1.13c D2Game+0x10A20 (object mode dispatch, called from 0x11070): stdcall (game, ppUnit, ctx), ret 0xC; returns 0 or 1.
// Written from the 1.13c disassembly as a rule over E (see d2game_179a0.h); every D2Common ordinal is reached through its D2Game import thunk.
// u = *ppUnit must be an object (type 4) whose class record rec = 0xA53E(u +4) is set, else 0. Reset: 0xB40E(u, 0), 0xB3C6/0xB3B4(u, 0, i) for
// i 0..2, 0xB0AE(u, 0), 0xB0A8(u, 0); mode = 0xEE90(u, ctx) unless ctx +0x30 is set, through 0xB090(u, mode). Overrides through 0xB090:
// 0xB438(u) with 0xAB62(u) gives 7, without it and 0xAFDC(u) == 0 gives 4; 0xB378(u) == 0 and 0xA898(u) == 6 gives 4; 0xB420(u) gives 7;
// 0xB426(u) gives 2. Switch on 0xA898(u) (1..9, anything else returns 0); k = 0xAFB2(ECX = 0xA5EC(u)) and the fallback 0x109B0 (EAX = k or the
// slot a callee was handed, ECX = ppUnit, ESI = ctx, stack = game). 1: 0x14AF0 (EAX = u, stack = ctx). 2: 0x107B0 (EAX = ctx, EBX = game,
// ESI = u), no fallback. 3: 0xB1AA(u, -1), 0xB37E(u), 0x140B0 (EBX = k, ESI = u, stack = u, ctx). 4: 0x103C0(ppUnit, ctx), 0x101E0 (EAX = ppUnit,
// EBX = &slot, ESI = ctx). 5: 0xB1AA, 0x141E0 (EAX = u, stack = ctx), the stat scale x2, 0x104F0 (EAX = ppUnit, EBX = ctx, ESI = &slot), 0x101E0.
// 6: reset pair, 0x154F0 (EAX = u), 0x104F0, 0x101E0. 7: 0xB1AA, 0xF370(game, u, ctx), the stat scale x3, 0x10730 (EAX = &slot, ECX = ppUnit,
// EBX = ctx), 0x104F0, 0x101E0. 8: reset pair, 0x14BF0 (EBX = k, ESI = u, stack = ctx). 9: reset pair, x = 0x14B30 (ESI = u, EDI = 1), y = (EDI = 0);
// both low words set write 0xB0AE(u, x), 0xB0A8(u, y), else the fallback. Stat scale: 0xA970(u) and level word 0xB37E(u) >= 1: 0x48 = 0xA42A(u,
// 0x48, 0), 0x49 = 0xA3E2(u, 0x49, 0), each times n capped at 255 (signed) through 0xA472(u, stat, v, 0). Tail: 0 unless 0 < mode < 10; level
// >= 100 runs 0xEBB0 (EBX = ctx, ESI = u); mode 1..3 runs 0xEC90 (EBX = ctx, EDI = u, stack = game); level >= 100, mode 1..9 but not 5 or 7,
// rec +0xF8 word w != 0: v = 0x14B50 (EBX = 1, ESI = u, EDI = 0, stack = 0, 1, 0, w); a nonzero low word runs 0xB40E(u, v) and
// 0xA9D0(0, 0, u, 0xB3AE(v & 0xFFFF), 0, 0). Returns 1. EDI arguments are logged as the capture's REGS `J` record (rva | 0x20000000, edi).
#pragma once
#include <cstdint>

namespace g10a20 {
template <class E> void Edi(E& e, uint32_t rva, uint32_t v)
{
    if (e.L[0] >= E::kLogMax) return;
    uint32_t* r = e.L + 1 + e.L[0]++ * 10;
    r[0] = rva | 0x20000000u; r[1] = E::isStk(v) ? 0xB3000000u : v;
    for (int i = 2; i < 10; i++) r[i] = 0;
}
template <class E> void Scale(E& e, uint32_t u, int32_t n)
{
    if (!e.h(0xA970, 0, 0, 0, 0, 0, u) || (e.h(0xB37E, 0, 0, 0, 0, 0, u) & 0xFFFF) < 1) return;
    const int32_t a = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, u, 0x48, 0), b = (int32_t)e.h(0xA3E2, 0, 0, 0, 0, 0, u, 0x49, 0);
    int32_t v = a * n; if (v >= 0xFF) v = 0xFF;
    e.h(0xA472, 0, 0, 0, 0, 0, u, 0x48, (uint32_t)v, 0);
    v = b * n; if (v >= 0xFF) v = 0xFF;
    e.h(0xA472, 0, 0, 0, 0, 0, u, 0x49, (uint32_t)v, 0);
}
template <class E> uint32_t ObjectModeDispatch(E& e, uint32_t game, uint32_t pp, uint32_t ctx)
{
    // 1.13c D2Game+0x10A20
    const uint32_t u = e.rd32(pp);
    if (!u || e.rd32(u) != 4) return 0;
    const uint32_t rec = e.h(0xA53E, 0, 0, 0, 0, 0, e.rd32(u + 4));
    if (!rec) return 0;
    auto mode = [&] { return e.h(0xA898, 0, 0, 0, 0, 0, u); };
    auto setMode = [&](uint32_t m) { e.h(0xB090, 0, 0, 0, 0, 0, u, m); };
    auto resetPair = [&] { e.h(0xB0AE, 0, 0, 0, 0, 0, u, 0); e.h(0xB0A8, 0, 0, 0, 0, 0, u, 0); };
    auto key = [&] { const uint32_t c = e.h(0xA5EC, 0, 0, 0, 0, 0, u); return e.h(0xAFB2, 0, c); };
    uint32_t slot = 0;
    auto fallback = [&](uint32_t a) { e.h(0x109B0, a, pp, 0, 0, ctx, game); };
    auto f104 = [&] { return e.h(0x104F0, pp, 0, 0, ctx, e.stk(&slot)); };
    auto f101 = [&] { return e.h(0x101E0, pp, 0, 0, e.stk(&slot), ctx); };
    e.h(0xB40E, 0, 0, 0, 0, 0, u, 0);
    for (uint32_t i = 0; i < 3; i++) { e.h(0xB3C6, 0, 0, 0, 0, 0, u, 0, i); e.h(0xB3B4, 0, 0, 0, 0, 0, u, 0, i); }
    resetPair();
    uint32_t m = e.h(0xEE90, 0, 0, 0, 0, 0, u, ctx);
    if (e.rd32(ctx + 0x30)) m = e.rd32(ctx + 0x30);
    setMode(m);
    if (e.h(0xB438, 0, 0, 0, 0, 0, u)) {
        if (e.h(0xAB62, 0, 0, 0, 0, 0, u)) setMode(7);
        else if (!e.h(0xAFDC, 0, 0, 0, 0, 0, u)) setMode(4);
    }
    if (!e.h(0xB378, 0, 0, 0, 0, 0, u) && mode() == 6) setMode(4);
    if (e.h(0xB420, 0, 0, 0, 0, 0, u)) setMode(7);
    if (e.h(0xB426, 0, 0, 0, 0, 0, u)) setMode(2);
    switch (mode()) {
    case 1: { const uint32_t k = key(); if (!e.h(0x14AF0, u, 0, 0, 0, 0, ctx)) fallback(k); break; }
    case 2: e.h(0x107B0, ctx, 0, 0, game, u); break;
    case 3: {
        e.h(0xB1AA, 0, 0, 0, 0, 0, u, 0xFFFFFFFFu);
        const uint32_t k = key();
        e.h(0xB37E, 0, 0, 0, 0, 0, u);
        if (!e.h(0x140B0, 0, 0, 0, k, u, u, ctx)) fallback(k);
        break;
    }
    case 4:
        slot = key();
        if (!e.h(0x103C0, 0, 0, 0, 0, 0, pp, ctx) && !f101()) fallback(slot);
        break;
    case 5:
        e.h(0xB1AA, 0, 0, 0, 0, 0, u, 0xFFFFFFFFu);
        slot = key();
        if (!e.h(0x141E0, u, 0, 0, 0, 0, ctx)) { Scale(e, u, 2); if (!f104() && !f101()) fallback(slot); }
        break;
    case 6:
        resetPair();
        slot = key();
        if (!e.h(0x154F0, u) && !f104() && !f101()) fallback(slot);
        break;
    case 7:
        e.h(0xB1AA, 0, 0, 0, 0, 0, u, 0xFFFFFFFFu);
        slot = key();
        if (!e.h(0xF370, 0, 0, 0, 0, 0, game, u, ctx)) {
            Scale(e, u, 3);
            if (!e.h(0x10730, e.stk(&slot), pp, 0, ctx) && !f104() && !f101()) fallback(slot);
        }
        break;
    case 8: { resetPair(); const uint32_t k = key(); if (!e.h(0x14BF0, 0, 0, 0, k, u, ctx)) fallback(k); break; }
    case 9: {
        resetPair();
        slot = key();
        const uint32_t x = e.h(0x14B30, 0, 0, 0, 0, u); Edi(e, 0x14B30, 1);
        const uint32_t y = e.h(0x14B30, 0, 0, 0, 0, u); Edi(e, 0x14B30, 0);
        if ((x & 0xFFFF) && (y & 0xFFFF)) { e.h(0xB0AE, 0, 0, 0, 0, 0, u, x); e.h(0xB0A8, 0, 0, 0, 0, 0, u, y); }
        else fallback(slot);
        break;
    }
    default: return 0;
    }
    if ((int32_t)mode() <= 0 || (int32_t)mode() >= 10) return 0;
    if ((e.h(0xB37E, 0, 0, 0, 0, 0, u) & 0xFFFF) >= 100) e.h(0xEBB0, 0, 0, 0, ctx, u);
    const int32_t m2 = (int32_t)mode();
    if (m2 > 0 && m2 <= 3) { e.h(0xEC90, 0, 0, 0, ctx, 0, game); Edi(e, 0xEC90, u); }
    if ((e.h(0xB37E, 0, 0, 0, 0, 0, u) & 0xFFFF) >= 100) {
        const uint32_t m3 = mode();
        if (m3 - 1 <= 8 && m3 != 5 && m3 != 7) {
            const uint32_t w = e.rd8(rec + 0xF8) | e.rd8(rec + 0xF9) << 8;
            if (w) {
                const uint32_t v = e.h(0x14B50, 0, 0, 0, 1, u, 0, 1, 0, w); Edi(e, 0x14B50, 0);
                if (v & 0xFFFF) {
                    e.h(0xB40E, 0, 0, 0, 0, 0, u, v);
                    const uint32_t s = e.h(0xB3AE, 0, 0, 0, 0, 0, v & 0xFFFF);
                    e.h(0xA9D0, 0, 0, 0, 0, 0, 0, 0, u, s, 0, 0);
                }
            }
        }
    }
    return 1;
}
}
