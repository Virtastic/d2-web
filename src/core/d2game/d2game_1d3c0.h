// 1.13c D2Game+0x1D3C0 (D2GAME_PETAI_PetMove): EAX = pUnit (the pet), stack = game, owner, motion, run, speed, steps (ret 0x18). Written from the 1.13c disassembly as a rule over E
// (see d2game_1c8a0.h). PD = 0xA4C0(owner) (player data), v11 / v12 = 0xB36C / 0xB372 (owner path x / y). pAi = (unit && type == 1 && unit+0x14) ? [unit+0x14 + 0x2c] : 0; the
// inline "velocity" writes are pAi+0x1C = speed (only when nonzero), pAi+0x20 = mode byte. Motion 0 walks one of eight neighbours of the owner's far point; 1 follows the owner's
// 20-entry position trail (PD+0xA0 head byte, x/y pairs at PD+0xA8); 2 idles or steps closer by chance; 3 teleports next to the owner; 4 and 5 use the pet iterator and the escape
// calls. The unit's type-2 state nodes (unit+0xDC list, +0x24 next) get 0x8E6F0 (EAX = node) after each failed move. A speed outside -126..126 reaches the retail fatal path (not modelled).
#pragma once
#include <cstdint>
#include <cstdlib>

namespace g1d3c0 {
template <class E> uint32_t Move(E& e, uint32_t U, uint32_t G, uint32_t O, uint32_t M, uint32_t Run, uint32_t Sp, uint32_t St, uint32_t dtA80)
{
    // 1.13c D2Game+0x1D3C0
    const uint32_t mode = e.rd32(O + 0x10), path = e.rd32(O + 0x2c);
    const uint32_t PD = e.h(0xA4C0, 0, 0, 0, 0, 0, O);
    const uint32_t v11 = e.h(0xB36C, 0, 0, 0, 0, 0, path), v12 = e.h(0xB372, 0, 0, 0, 0, 0, path);
    auto pai = [&]() -> uint32_t { if (U && e.rd32(U) == 1) { const uint32_t p = e.rd32(U + 0x14); if (p) return e.rd32(p + 0x2c); } return 0; };
    auto nodes = [&]() { if (U) for (uint32_t n = e.rd32(U + 0xDC); n;) { const uint32_t nx = e.rd32(n + 0x24); if (e.rd8(n) == 2) e.h(0x8E6F0, n); n = nx; } };
    auto idle = [&](uint32_t n) { e.h(0xF22C0, G, 0, 0, 0, U, n); };
    auto go = [&](uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5) { return e.h(0xF2540, 0, G, U, 0, 0, s0, s1, s2, s3, s4, s5); };
    auto w32 = [&](uint32_t p, uint32_t off, uint32_t v) { if (p) e.wr32(p + off, v); };
    auto speed = [&](uint32_t p, uint32_t sp, uint32_t m) { if (sp) w32(p, 0x1C, sp); if (p) e.wr8(p + 0x20, m); };
    auto range = [&](uint32_t sp) { if ((int32_t)sp < -126 || (int32_t)sp > 126) abort(); };
    switch (M) {
    case 0: {
        static const uint8_t T[8] = { 4, 3, 2, 1, 0, 7, 6, 5 };
        uint32_t slot = St;
        slot = (slot & ~0xFFu) | (e.h(0xA7A8, 0, 0, 0, 0, 0, O, v11, v12) & 0xFF);
        slot = (slot & ~0xFFu) | (e.h(0xAD1E, 0, 0, 0, 0, 0, slot) & 0xFF);
        const uint32_t Y = e.h(0x1130, 0, O), X = e.h(0x1100, 0, O);
        const uint32_t a4 = e.h(0xAD66, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, O), X, Y);
        for (int i = 0; i < 8; i++) {
            uint32_t xo = 0, yo = 0;
            e.h(0xAC88, 0, 0, 0, 0, 0, T[slot & 7], e.stk(&xo), e.stk(&yo));
            const uint32_t nX = v11 + 8 * xo, nY = v12 + 8 * yo;
            if (e.h(0xAD66, 0, 0, 0, 0, 0, e.h(0xA478, 0, 0, 0, 0, 0, O), nX, nY) == a4) {
                range(Sp);
                speed(pai(), Sp, 0x28);
                if (go(0, 2, nX, nY, 1, 4)) return 1;
            }
            slot = (slot & ~0xFFu) | (((slot & 0xFF) + 1) & 7);
        }
        nodes();
        e.h(0xF29B0, 4, 0, 0, 0, 0, G, U);
        return 1;
    }
    case 1: {
        if (mode == 2) { speed(pai(), 0, 0x4D); go(0, 2, v11, v12, 1, 0); return 1; }
        uint32_t sp = Sp;
        if (!sp) sp = e.h(0x1180, 0xF, U + 0x20) + 0x32;
        uint32_t v29 = e.rd8(PD + 0xA0);
        const uint32_t prm = e.h(0x1DD0, U);
        bool vel = false;
        auto walk = [&](uint32_t X, uint32_t Y) { return Run ? go(0, 0xF, X, Y, 1, 0) : go(0, 2, X, Y, 1, 4); };
        for (int i = 0; i < 20; i++) {
            v29 = v29 ? (v29 - 1) & 0xFF : 19;
            const uint32_t Y = e.rd32(PD + v29 * 8 + 0xAC), X = e.rd32(PD + v29 * 8 + 0xA8);
            if (e.h(0xB07B0, U, 0, 0, 0, 0, X, Y) <= 5) continue;
            range(sp);
            speed(pai(), sp, 0x4D);
            if (go(0, 2, X, Y, 1, 4)) return 1;
            nodes();
            { const uint32_t p = pai(); w32(p, 0x18, 0xF); speed(p, sp, 0x4D); }
            if (walk(X, Y)) return 1;
            nodes();
            if (!vel) {
                vel = true;
                w32(prm, 0x18, 1); speed(prm, sp, 0x4D);
                if (walk(X, Y)) return 1;
                nodes();
            }
        }
        int32_t d = (int32_t)(e.h(0xB0A20, 0, 0, 0, 0, 0, O) >> 2);
        if (d < 4) d = 4;
        w32(pai(), 0x1C, 0xF);
        e.h(0xF29B0, (uint32_t)d, 0, 0, 0, 0, G, U);
        return 1;
    }
    case 2: {
        if ((int32_t)e.h(0x1180, 100, U + 0x20) >= 10) { idle(0xF); return 1; }
        const uint32_t r = e.h(0x1180, 3, U + 0x20);
        if (e.h(0xF29B0, (r & ~0xFFu) | ((r + 3) & 0xFF), 0, 0, 0, 0, G, U)) return 1;
        nodes();
        { const uint32_t p = pai(); if (p) e.wr8(p + 0x20, 0x28); }
        go(0, 2, v11, v12, 1, 0);
        return 1;
    }
    case 3: {
        const uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, O);
        const uint32_t Y = e.h(0x1130, 0, O), X = e.h(0x1100, 0, O);
        const uint32_t r = e.h(0xAF28, 0, 0, 0, 0, 0, room, X, Y);
        const uint32_t cls = (int32_t)dtA80 > 0x16B ? 0x16Bu : 0xFFFFFFFFu;
        uint32_t nX = 0, nY = 0;
        if (!e.h(0x47450, 0, 0, 0, 0, 0, G, room, r, cls, e.stk(&nX), e.stk(&nY), 0)) return 0;
        const uint32_t room2 = e.h(0x1280, room, 0, 0, nX);
        if (!room2) return 0;
        if (!e.h(0xE19C0, nY, nX, room2, 0, 0, G, U, 0, 0)) return 0;
        e.h(0xA4DE, 0, 0, 0, 0, 0, U);
        if (U) e.wr32(U + 0xC8, e.rd32(U + 0xC8) | 0x10000);
        idle(5);
        return 1;
    }
    case 4: {
        uint32_t st[3] = { U, 0, 0 };
        e.ed(G);
        e.h(0x96AF0, 0, 0, 0, 0, 0, O, e.stk(st));
        e.edi(0x96AF0, G);
        if ((int32_t)st[2] < 1) { idle(0xF); return 1; }
        nodes();
        if (e.h(0xF2D80, O, 0, 0, 0, 0, G, U, St, 1)) return 1;
        nodes();
        e.h(0xF29B0, St & 0xFF, 0, 0, 0, 0, G, U);
        return 1;
    }
    case 5: {
        w32(pai(), 0x18, 7);
        if (e.h(0xF2870, 0, O, 0, 0, 0, G, U)) return 1;
        nodes();
        if (e.h(0xF2D80, O, 0, 0, 0, 0, G, U, St, 1)) return 1;
        { const uint32_t p = pai(); if (p) e.wr8(p + 0x20, 0x28); }
        go(0, 2, v11, v12, 1, 0);
        return 1;
    }
    default: return 0;
    }
}
}
