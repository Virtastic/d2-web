// 1.13c D2Multi+0x13280: EBX = a game-info record (+0xC elapsed seconds, +0x10 level, +0x11 level spread, +0x12 max players, +0x14 player count,
// +0x2C description, +0x4C class u16[], +0x6C level u16[], +0x8C names 0x10 each); cc4 = the details list control (DAT_6fa09cc4, 0 = nothing to do).
// Clears it (7CB0), then adds one text line per fact through 7CBC(ECX = control, EDX = text): description plus a blank, "Elapsed Time: h:mm:ss", the level
// or level range when the spread is below 100, "Up to %d Players" when 1..7, a blank, and per named player the name followed by "Level %d <class>"
// through 7C5C(ECX, EDX, 5). Ends with 7CD4(ECX, 0).
#include "d2multi_env.h"
#include <cstring>

void d2multi_13280(const D2mEnv& e, const uint8_t* w, uint32_t cc4) {
    if (!cc4) return;
    char buf[256];
    const uint32_t blank = D2M_MOD(0x175d7);
    auto fmt = [&](uint32_t rva, uint32_t a, uint32_t b = 0) { d2m_call(e, 0x1000, 0, 0, 0, 0, 0, d2m_ptr(buf), D2M_MOD(rva), a, b); };
    auto line = [&](uint32_t p) { d2m_call(e, 0x7cbc, 0, cc4, p); };
    d2m_call(e, 0x7cb0, 0, cc4);
    if (w[0x2c]) { line(d2m_ptr(w + 0x2c)); line(blank); }
    uint32_t el; memcpy(&el, w + 0xc, 4);
    fmt(0x175bc, el / 3600, (el % 3600) / 60);
    line(d2m_ptr(buf));
    uint32_t spread = w[0x11], lvl = w[0x10];
    if (spread < 100) {
        if (!spread) fmt(0x175a0, lvl);
        else {
            int32_t lo = (int32_t)lvl - (int32_t)spread;
            if (lo < 2) lo = 1;
            uint32_t hi = spread + lvl;
            if (hi > 0x62) hi = 0x63;
            fmt(0x175ac, (uint32_t)lo, hi);
        }
        line(d2m_ptr(buf));
    }
    uint32_t mp = w[0x12];
    if (mp && mp < 8) { fmt(0x1758c, mp); line(d2m_ptr(buf)); }
    line(blank);
    int32_t n; memcpy(&n, w + 0x14, 4);
    static const uint16_t cls[7] = { 0x757c, 0x7568, 0x7550, 0x753c, 0x7528, 0x7518, 0x7504 };
    for (int32_t i = 0; i < n; i++) {
        const uint8_t* nm = w + 0x8c + 0x10 * i;
        if (!*nm) continue;
        line(d2m_ptr(nm));
        uint16_t c, l; memcpy(&c, w + 0x4c + 2 * i, 2); memcpy(&l, w + 0x6c + 2 * i, 2);
        fmt(c < 7 ? 0x10000u + cls[c] : 0x175a0, l);
        d2m_call(e, 0x7c5c, 0, cc4, d2m_ptr(buf), 0, 0, 5);
    }
    d2m_call(e, 0x7cd4, 0, cc4, 0);
}
