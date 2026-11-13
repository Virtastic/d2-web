// 1.13c D2Client+0x29300 -- bottom-bar left-button handler (stdcall msg, ret 4; x = word at msg+0xC, y = word at msg+0xE,
// results at msg+0x18 / +0x1C). Held-off input, no player or a dead player: msg+0x18 = 0 only. Otherwise it clears the
// two click words (0x11C45C, 0x11C458) and splits on the pressed flag 0x11C478: not pressed, a press on one of the
// two side buttons (left x 0x75..0xA5, right W-0xA5..W-0x75, y H-0x30..H) opens it through 0xC2790 when the panel word
// 0xFAD8C is set; pressed (0x11C46C set or not), the release rules: the left and right buttons, the centre button
// (W/2-8..W/2+5), the toggle at W/2-0x91..W/2-0x80 flipping 0x11C3EC and the strip W/2+0x17..W/2+0x91. Every exit
// clears 0x11C4A4 / 0x11C4AC. The fatal path (line 0x139B) falls through to the entry scan, as retail does.
#include "cl_tbl.h"
extern "C" uint32_t d2c_29300(uint32_t msg, const cl::Env& e) {
    using clt::G; using clt::S;
    const int32_t x = (int32_t)(e.R(msg + 0xC) & 0xffffu), y = (int32_t)(e.R(msg + 0xE) & 0xffffu);
    const uint32_t player = G(e, 0x11BBFC);
    if (G(e, 0x11C3A4) || !player || e.R(player + 0x10) == 0x11) { e.W(msg + 0x18, 0); return 0; }
    S(e, 0x11C45C, 0);
    S(e, 0x11C458, 0);
    const int32_t W = (int32_t)G(e, 0xDBC48), Hh = (int32_t)G(e, 0xDBC4C);
    const int32_t hw = (W - (W >> 31)) >> 1;
    auto clearA = [&] { S(e, 0x11C4A4, 0); S(e, 0x11C4AC, 0); };
    auto endB = [&] { S(e, 0x11C478, 0); clearA(); };
    auto c2790 = [&](uint32_t c, uint32_t d) { e.H(0xC2790, 0, 0, 0, 0, 0, 0, c, d); };
    auto pre = [&] {
        const uint32_t m = G(e, 0x11B864);
        if (m == 6) e.H(0xBEB90);
        else if (m == 8) e.H(0x14940, 0, 0, 0, 0, 0, 0x4C, 0xFFFFFFFFu);
    };
    auto c28260 = [&] { e.H(0x28260, e.R(player + 0x60), 0, 0, 0, 0, (uint32_t)y, (uint32_t)x); };
    auto getter = [&](uint32_t rva) { const uint32_t s = e.H(0xD0B4, 0x6FB80864u); e.H(rva, s, 0, 0, 0, 0, 2); };
    const bool left = x >= 0x75 && x <= 0xA5, right = x >= W - 0xA5 && x <= W - 0x75;
    const bool band = y >= Hh - 0x30 && y <= Hh;
    if (G(e, 0x11C478) == 0) {
        if ((left || right) && band && G(e, 0xFAD8C)) c2790(3, 1);
        S(e, 0x11C478, 0);
        e.W(msg + 0x18, 0);
        clearA();
        return 0;
    }
    if (G(e, 0x11C46C) == 0) {
        if (y <= Hh - 0x30) { e.W(msg + 0x18, 0); clearA(); return 0; }
        auto tail = [&] {   // 0x2984C
            S(e, 0x11C478, 0); S(e, 0xF5388, 0);
            e.W(msg + 0x18, 1); e.W(msg + 0x1C, 0); e.H(0xBE84, msg);
            clearA();
            return 0u;
        };
        if (x >= hw + 0x17 && x <= hw + 0x91 && y >= Hh - 0x27 && y <= Hh - 0xA) { pre(); c28260(); return tail(); }
        if (left && y <= Hh) { pre(); c2790(3, G(e, 0xFAD8C) ? 1 : 0); S(e, 0x11C420, 1); return tail(); }
        if (right && y <= Hh) { pre(); c2790(3, G(e, 0xFAD8C) ? 1 : 0); S(e, 0x11C420, 0); return tail(); }
        if (x >= hw - 0x91 && x <= hw - 0x80 && y >= Hh - 0x1C && y <= Hh - 8) { S(e, 0x11C3EC, G(e, 0x11C3EC) == 0); return tail(); }
        if (x >= hw - 8 && x <= hw + 5 && y >= Hh - 0x27 && y <= Hh - 0xD) {
            c2790(0x15, G(e, 0xFADD4) ? 1 : 2);
            if (G(e, 0x11C4A4)) S(e, 0x11C4A8, G(e, 0x11C4A8) == 0);
        }
        return tail();
    }
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg)) {
        pre(); c28260();
        e.W(msg + 0x18, 1); e.W(msg + 0x1C, 0); e.H(0xBE84, msg);
        S(e, 0x11C478, 0); clearA();
        return 0;
    }
    if (x >= hw - 8 && x <= hw + 5 && y >= Hh - 0x27 && y <= Hh - 0xD) {
        const uint32_t old = G(e, 0xFADD4);
        e.H(0xBF8C);
        const uint32_t cnt = G(e, 0xDD5D0);
        if (old && cnt == 0x26) {
            S(e, 0xFADD4, 0);
            getter(0x3CC60);
            endB();
            return 0;
        }
        if (cnt != 0x26) clt::assertAt(e, 0x139B);
        uint32_t edx = G(e, 0xFADD4);
        if (edx == 0) {
            if (G(e, 0x113A68)) { endB(); return 0; }
            uint32_t i = 0;
            do {
                if (G(e, 0xFAD80 + i * 4) == 1) {
                    const uint32_t st = e.R(G(e, 0xDD538 + i * 4) + 0x54);
                    if (st > 4 || st == 2 || st == 3) { endB(); return 0; }
                    if (st == 1) c2790(i, 1);
                    else if (st == 4 && G(e, 0x119721)) { e.H(0x47B20); e.Edj(0x47B20, G(e, 0x11971D)); }
                }
                ++i;
            } while (i < G(e, 0xDD5D0));
            edx = G(e, 0xFADD4);
        }
        if (!player) { edx = edx == 0; S(e, 0xFADD4, edx); }
        else if (e.R(player) == 0 && !e.H(0xA5440, 0, 0, 0, 0, 0, player)) { edx = edx == 0; S(e, 0xFADD4, edx); }
        if (edx) getter(0x3CCC0);
        endB();
        return 0;
    }
    if (left && band) { c2790(3, 2); S(e, 0x11C420, 1); endB(); return 0; }
    if (right && band) { c2790(3, 2); S(e, 0x11C420, 0); endB(); return 0; }
    if (x >= hw - 0x91 && x <= hw - 0x80 && y >= Hh - 0x1C && y <= Hh - 8) S(e, 0x11C3EC, G(e, 0x11C3EC) == 0);
    endB();
    return 0;
}
