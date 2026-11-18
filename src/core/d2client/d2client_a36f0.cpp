// 1.13c D2Client+0xA36F0 (333 B, ECX = menu, EAX = kind, 6 stack words, ret 0x18): menu_add_entry.
// Stack: text (UCS-2), cmd, a2, a3, a4, sel. Entry n = menu[+0x50] (stride 0x110 from +0x68: text 0x77 chars, +0xF0 cmd, +0xF4 0,
// +0xFC a2, +0x100 a3, +0x104 kind, +0x108 a4, +0x10C sel). menu[+0x4C] counts selectable entries. Layout: 0xA3580 (EAX = menu)
// when menu[+0x58], else 0xA32D0 (EAX = n). gr = window 0x11963C..0x11964B (menu rect), gf = 0x11C2FC (dirty). 0xA3370 (ECX =
// menu) when menu[+0x44] == -1 and sel. The Unicode::strlen/strncpy imports are done inline. n >= 10 and kind 3 on a menu with
// [+0x5C] == 1 are retail's fatal D2Lang message + _exit(-1); the vectors stay off them.
#include "cl_env.h"
extern "C" uint32_t d2c_a36f0(uint32_t m, uint32_t kind, uint32_t text, uint32_t cmd, uint32_t a2, uint32_t a3, uint32_t a4,
                          uint32_t sel, uint32_t gr, uint32_t gf, const cl::Env& e) {
    const uint32_t n = e.R(m + 0x50);
    e.W(m + 0x50, n + 1);
    const uint32_t ent = m + n * 0x110;
    e.W(ent + 0x158, cmd); e.W(ent + 0x15c, 0); e.W(ent + 0x164, a2); e.W(ent + 0x168, a3);
    e.W(ent + 0x16c, kind); e.W(ent + 0x170, a4); e.W(ent + 0x174, sel);
    uint32_t len = 0;
    while (e.R(text + 2 * len) & 0xffff) ++len;
    uint32_t cnt = len + 1;
    if ((int32_t)cnt > 0x77) cnt = 0x77;
    auto put16 = [&](uint32_t a, uint32_t v) { e.W(a & ~3u, (a & 2) ? ((e.R(a & ~3u) & 0xffff) | (v << 16)) : ((e.R(a) & 0xffff0000u) | v)); };
    for (uint32_t i = 0; i < cnt; ++i) {
        const uint32_t c = e.R(text + 2 * i) & 0xffff;
        put16(ent + 0x68 + 2 * i, c);
        if (!c) break;
    }
    put16(m + (n * 0x88 + cnt) * 2 + 0x68, 0);
    if (sel) e.W(m + 0x4c, e.R(m + 0x4c) + 1);
    if (e.R(m + 0x58)) e.H(0xa3580, 0, 0, 0, 0, 0, m);
    else e.H(0xa32d0, 0, 0, 0, 0, 0, n);
    const uint32_t x = e.R(m + 0x2c), y = e.R(m + 0x30);
    e.W(gr + 0, x); e.W(gr + 4, y); e.W(gr + 8, e.R(m + 0x34) + x); e.W(gr + 12, e.R(m + 0x38) + y);
    e.W(gf, 1);
    if (e.R(m + 0x44) == 0xffffffffu && sel) return e.H(0xa3370, 0, 0, 0, 0, 0, 0, m);
    return e.R(m + 0x44);
}
