// 1.13c D2Client+0x47500 -- the vendor confirm menu (Buy / Sell / Repair / Identify / merc-replace).
// Typed seam: every callee is one Env hook carrying the registers and stack words retail pushes at that call
// (0xA3E40 11 stack words, 0xA36F0 EAX = colour, ECX = menu, 6 stack words, 0x914F0 3 stack words, 0x46230 EAX = &out, ECX = unit, 3 stack words,
// string lookup = D2Lang 10003 with ECX = id). The inlined Unicode default constructors (IAT 0xCEB00), Unicode::strlen (0xCEAB0),
// wsprintfA("%d"), Unicode::toUnicode(dst, src, 9) (0xCEAC0) and Unicode::strcpy (0xCEAB8) are reproduced here.
#include <cstdint>
#include <cstddef>
#include <emscripten.h>

#include "d2client_47500.h"

namespace d2client_47500 {
static inline uint32_t rd(uint32_t a) { return *(const uint32_t*)(uintptr_t)a; }
static const uint32_t CB_YES = 0x6FAF7480, CB_CLOSE = 0x6FAF61D0;

// 1.13c D2Client+0x47500
void sub_47500(const Env& e, State& s) {
    if (s.box) { e.fatal(e.c, 0x151b); return; }
    s.g9663 = 4;
    const uint32_t mw[11] = { s.a828, s.a824, CB_CLOSE, 0, 0, 1, 1, 0, 0, 0, 1 };
    s.box = e.menu(e.c, mw);
    if (!s.box) { e.fatal(e.c, 0x529); return; }
    *(uint32_t*)(uintptr_t)(s.box + 0x5c) = 1;
    if (s.mode - 1u > 4u) { e.fatal(e.c, 0x1550); return; }
    static const uint32_t ids[5] = { 0xd14, 0xd13, 0xd17, 0xd16, 0xd18 };   // jump table 0x6FAF77F4
    auto line = [&](uint32_t colour, const char16_t* t, uint32_t w1, uint32_t w3, uint32_t cb, uint32_t sel) {
        const uint32_t w[6] = { (uint32_t)(uintptr_t)t, w1, 0, w3, cb, sel };
        e.line(e.c, colour, s.box, w);
    };
    line(4, e.str(e.c, ids[s.mode - 1]), 0x15, 1, 0, 0);          // title, ebp = 1
    if (s.mode != 5) {
        const uint32_t carried = s.mode == 1 ? 0 : 1;             // ebp: the xor at 0x6FAF75C8 is only reached from mode 1
        uint32_t u = s.hash[s.id & 0x7f];
        while (u && rd(u + 0xc) != s.id) u = rd(u + 0xe4);
        if (!u) return;
        if (rd(u) != 4) { e.fatal(e.c, 0x5a); return; }
        char16_t nb[192] = {};
        e.name(e.c, u, nb, 0xbf);
        int n = 0; while (nb[n]) ++n;
        for (int i = 0; i < n; ++i) if (nb[i] == 10) { nb[i] = 0; break; }
        line(4, nb, 0xf, 1, CB_YES, 0);
        s.gold[0] = 0;
        uint32_t amount = 0;
        e.cost(e.c, &amount, u, carried, s.gold, 0x28);
        char a[16]; int m = 0; uint32_t mag = (int32_t)amount < 0 ? 0u - amount : amount;   // wsprintfA("%d", amount)
        char r[12]; int k = 0; do { r[k++] = (char)('0' + mag % 10); mag /= 10; } while (mag);
        if ((int32_t)amount < 0) a[m++] = '-';
        while (k) a[m++] = r[--k];
        a[m] = 0;
        char16_t w2[10] = {};
        {   // Unicode::win2Unicode(w2, a, 9): copy up to 9 chars; a longer source loses its 9th to the NUL
            int i = 0;
            while (i < 9 && a[i]) { w2[i] = (uint8_t)a[i]; ++i; }
            w2[a[i] ? i - 1 : i] = 0;
        }
        int i = 0; do s.gold[i] = w2[i]; while (w2[i++]);             // Unicode::strcpy(0x6FBD230D, w2)
        line(4, e.str(e.c, 0xd12), 0xf, 1, CB_YES, 0);
        line(4, s.gold, 0xf, 1, CB_YES, 0);
    }
    line(0, e.str(e.c, 0xd10), 0xf, 1, CB_YES, 1);
    line(0, e.str(e.c, 0xd11), 0xf, 1, CB_CLOSE, 1);
}
}
