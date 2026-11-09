// 1.13c D2Client+0xA36F0: append an entry to a popup menu. ECX = menu, EAX = kind, stack text cmd a2 a3 a4 sel (ret 0x18).
// Layout seams: 0xA32D0 (menu+0x58 == 0), 0xA3580 (else), 0xA3370 (select first).
// gr = the rect at 0x11963C (x, y, x+w, y+h as dwords 0, 1, 2, 3 = 963C, 9640, 9644, 9648), gf = the flag at 0x11C2FC.
#include "cl_env.h"
extern "C" uint32_t d2c_a36f0(uint32_t m, uint32_t kind, uint32_t text, uint32_t cmd, uint32_t a2, uint32_t a3, uint32_t a4,
                              uint32_t sel, uint32_t gr, uint32_t gf, const cl::Env& e) {
    const int32_t i = (int32_t)e.R(m + 0x50);
    if (i > 9) __builtin_trap();
    e.W(m + 0x50, (uint32_t)(i + 1));
    const uint32_t en = m + (uint32_t)i * 0x110;
    e.W(en + 0x158, cmd); e.W(en + 0x15c, 0);
    e.W(en + 0x164, a2); e.W(en + 0x168, a3);
    if (kind == 3 && e.R(m + 0x5c) == 1) __builtin_trap();
    e.W(en + 0x16c, kind); e.W(en + 0x170, a4); e.W(en + 0x174, sel);
    const uint16_t* t = (const uint16_t*)(uintptr_t)text;
    int32_t n = 0; while (t[n]) ++n;
    ++n; if (n > 0x77) n = 0x77;
    uint16_t* d = (uint16_t*)(uintptr_t)(en + 0x68);
    int32_t k = 0; for (; k < n && t[k]; ++k) d[k] = t[k];
    for (; k < n; ++k) d[k] = 0;
    *(uint16_t*)(uintptr_t)(m + (uint32_t)(i * 0x88 + n) * 2 + 0x68) = 0;
    if (sel) e.W(m + 0x4c, e.R(m + 0x4c) + 1);
    if (!e.R(m + 0x58)) e.H(0xa32d0, 0, 0, 0, 0, 0, (uint32_t)i); else e.H(0xa3580, 0, 0, 0, 0, 0, m);
    const uint32_t y = e.R(m + 0x30), x = e.R(m + 0x2c);
    e.W(gr, x); e.W(gr + 4, y);
    e.W(gr + 8, e.R(m + 0x34) + x); e.W(gr + 12, e.R(m + 0x38) + y);
    e.W(gf, 1);
    // void in the source; EAX at the ret is [m+0x44] (just compared), or the 0xA3370 result when it is called
    const uint32_t r44 = e.R(m + 0x44);
    if (r44 == 0xffffffffu && sel) return e.H(0xa3370, 0, 0, 0, 0, 0, 0, m);
    return r44;
}
