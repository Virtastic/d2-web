// 1.13c D2Client+0xBCEA0: the character stats panel draw. void(void), plain ret.
// Written from the disassembly. Every import and module callee goes through e.H with the registers and stack words the retail
// call site passes; the D2Lang string helpers (Unicode ctor, strncpy, strlen, win2Unicode, unicodenwidth) and USER32 wsprintfA
// run inline here, as they run for real in retail. Module globals are reached through g.at(rva) (the address that holds the
// global's bytes); the frame is one byte array laid out as retail's 0x370-byte frame, so the stack buffers handed to the
// callees are the same buffers.
#include <cstdint>
#include <cstring>
#include "cl_env.h"
struct D2CBCEA0G { void* ctx; uint32_t (*at)(void* ctx, uint32_t rva); };
namespace {
constexpr uint32_t kGfxDraw = 0xd096, kStr = 0xd1ec, kHover = 0xd384, kFont = 0xd3a8, kWidth = 0xd35a, kText = 0xd37e,
    kMeasure = 0xd3b4, kLang = 0xd1e6, kNum = 0xd1f2, kStatBase = 0xc250, kStat = 0xc1c6;
struct Lbl { int32_t x0, y, x1; uint16_t id; };
// .data 0xDD5E0 (stride 0x12): the 15 labels
constexpr Lbl kLabels[15] = {
    { 11, 44, 52, 0xfd9 }, { 65, 44, 180, 0xfda }, { 193, 44, 308, 0xfdb }, { 10, 97, 73, 0xfdc }, { 10, 160, 73, 0xfde },
    { 174, 207, 268, 0xfe0 }, { 10, 245, 73, 0xfe2 }, { 174, 245, 228, 0xfe3 }, { 174, 269, 228, 0xfe4 }, { 10, 307, 73, 0xfe5 },
    { 174, 307, 228, 0xfe6 }, { 190, 346, 268, 0xfe7 }, { 190, 370, 268, 0xfe8 }, { 190, 395, 268, 0xfe9 }, { 190, 419, 268, 0xfea } };
struct Row { int32_t x0, y, x1; uint32_t stat; };
// .data 0xDD6F0 (stride 0x10): the 18 value rows
constexpr Row kRows[18] = {
    { 13, 59, 53, 12 }, { 67, 59, 180, 13 }, { 195, 59, 308, 30 }, { 77, 99, 112, 0 }, { 77, 161, 112, 2 }, { 273, 209, 307, 31 },
    { 77, 247, 112, 3 }, { 232, 246, 267, 11 }, { 273, 246, 308, 10 }, { 232, 270, 267, 7 }, { 273, 270, 308, 6 }, { 77, 308, 112, 1 },
    { 232, 308, 267, 9 }, { 273, 308, 308, 8 }, { 273, 348, 307, 39 }, { 273, 372, 307, 43 }, { 273, 396, 307, 41 }, { 273, 420, 307, 45 } };
struct P {
    const cl::Env& e; const D2CBCEA0G& g; uint8_t* F;
    uint32_t A(uint32_t off) const { return (uint32_t)(uintptr_t)(F + off); }
    uint32_t& D(uint32_t off) const { return *(uint32_t*)(void*)(F + off); }
    char16_t* Wb(uint32_t off) const { return (char16_t*)(void*)(F + off); }
    char* Nb(uint32_t off) const { return (char*)(F + off); }
    uint32_t G(uint32_t rva) const { return e.R(g.at(g.ctx, rva)); }
    uint32_t G8(uint32_t rva) const { return e.R8(g.at(g.ctx, rva)); }
    void SetG(uint32_t rva, uint32_t v) const { e.W(g.at(g.ctx, rva), v); }
    int32_t X() const { return (int32_t)G(0x11b9a0); }
    int32_t YH() const { return (int32_t)(G(0x11b9a4) + G(0xdbc4c)); }
    uint32_t str(uint32_t id) const { return e.H(kStr, 0, 0, 0, 0, 0, 0, id); }
    uint32_t font(uint32_t f) const { return e.H(kFont, 0, 0, 0, 0, 0, 0, f); }
    int32_t width(uint32_t s) const { return (int32_t)e.H(kWidth, 0, 0, 0, 0, 0, 0, s); }
    void text(uint32_t s, int32_t x, int32_t y, uint32_t col) const { e.H(kText, (uint32_t)y, col, 0, 0, 0, 0, s, (uint32_t)x); }
    // centred in [x, x+span): x += (span - w) >> 1 when the text is narrower
    int32_t centre(uint32_t s, int32_t x, int32_t span) const { const int32_t w = width(s); return w < span ? x + ((span - w) >> 1) : x; }
    void cel(uint32_t handle, uint32_t frame, int32_t x, int32_t y) const {
        D(0x2c + 13 * 4) = handle; D(0x2c) = frame;
        e.H(kGfxDraw, A(0x2c), (uint32_t)x, (uint32_t)y, 0xffffffffu, 5);
    }
    void ctor(uint32_t off, uint32_t n) const { for (uint32_t i = 0; i < n; ++i) Wb(off)[i] = 0; }
    // D2Lang win2Unicode(dst, src, n): widen bytes until NUL or n; a cut string loses its last char
    void w2u(uint32_t dst, const char* s, int32_t n) const {
        char16_t* d = Wb(dst); int32_t i = 0;
        while (i < n && s[i]) { d[i] = (char16_t)(uint8_t)s[i]; ++i; }
        if (s[i]) d[i - 1] = 0; else d[i] = 0;
    }
    void w2uA(uint32_t dst, uint32_t src, int32_t n) const {   // the same over a module/blob address
        char16_t* d = Wb(dst); int32_t i = 0;
        while (i < n && e.R8(src + (uint32_t)i)) { d[i] = (char16_t)e.R8(src + (uint32_t)i); ++i; }
        if (e.R8(src + (uint32_t)i)) d[i - 1] = 0; else d[i] = 0;
    }
    void itoa(uint32_t dst, int32_t v) const {   // wsprintfA "%i" / "%ld"
        char t[16]; int n = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
        do { t[n++] = (char)('0' + u % 10); u /= 10; } while (u);
        char* d = Nb(dst); if (v < 0) *d++ = '-';
        while (n) *d++ = t[--n];
        *d = 0;
    }
};
// D2Lang unicodenwidth: UTF-8 characters in the first n bytes (an invalid lead byte counts as nothing and is skipped)
int32_t clen(const cl::Env& e, uint32_t p, int32_t n) {
    static const uint32_t T[6][4] = { { 0x80, 0, 0x7f, 0 }, { 0xe0, 0xc0, 0x7ff, 0x80 }, { 0xf0, 0xe0, 0xffff, 0x800 },
        { 0xf8, 0xf0, 0x1fffff, 0x10000 }, { 0xfc, 0xf8, 0x3ffffff, 0x200000 }, { 0xfe, 0xfc, 0x7fffffff, 0x4000000 } };
    if (n <= 0) return -1;
    const uint32_t c0 = e.R8(p); uint32_t l = c0;
    for (int32_t k = 0; k < 6; ++k) {
        const int32_t cnt = k + 1;
        if ((c0 & T[k][0]) == T[k][1]) return (l & T[k][2]) >= T[k][3] ? cnt : -1;
        if (n <= cnt) return -1;
        const uint32_t c = e.R8(p + (uint32_t)cnt) ^ 0x80;
        if (c & 0xc0) return -1;
        l = l << 6 | c;
    }
    return -1;
}
uint32_t nwidth(const cl::Env& e, uint32_t s, int32_t n) {
    uint32_t cnt = 0;
    while (n > 0 && e.R8(s)) {
        const int32_t k = clen(e, s, n);
        if (k == -1) { ++s; --n; continue; }
        for (int32_t i = 0; i < k; ++i, ++s) if (!e.R8(s)) return cnt;
        ++cnt; n -= k;
    }
    return cnt;
}
}  // namespace
// 1.13c D2Client+0xBCEA0
extern "C" void d2cl_bcea0(const cl::Env& e, const D2CBCEA0G& g) {
    uint8_t frame[0x380];
    memset(frame, 0, sizeof frame);
    const P p{ e, g, frame };
    uint8_t* F = frame;
    (void)F;
    p.D(0x1c) = p.G(0x11bbfc);
    const uint32_t unit = p.D(0x1c);
    // the four panel cels
    p.D(0x10) = 0; p.D(0x6c) = 0;
    const uint32_t panel = p.G(0x11a7c8);
    p.cel(panel, 0, p.X(), p.YH() - 0xe0);
    p.cel(panel, 1, p.X() + 0x100, p.YH() - 0xe0);
    p.cel(panel, 2, p.X(), p.YH() - 0x30);
    p.cel(panel, 3, p.X() + 0x100, p.YH() - 0x30);
    const int32_t mx = (int32_t)p.G(0x11b828), my = (int32_t)p.G(0x11b824);
    p.D(0x14) = (uint32_t)mx; p.D(0x24) = (uint32_t)my;
    p.ctor(0x31c, 0x32);
    if (mx >= p.X() + 0x80 && mx <= p.X() + 0xa0 && my >= p.YH() - 0x5c && my <= p.YH() - 0x3c) {
        const int32_t x = p.X() + 0x8f, y = p.YH() - 0x5f;
        const uint32_t s = p.str(0x1030);
        e.H(kHover, (uint32_t)y, 0, 1, 0, 0, 0, s, (uint32_t)x);
    }
    // unspent stat points: the badge, two caption lines and the count
    if (e.H(kStatBase, unit, 4, 0)) {
        p.cel(p.G(0x11a784), 0, p.X() + 3, p.YH() - 0x74);
        p.ctor(0x78, 10);
        p.font(6);
        for (uint32_t k = 0; k < 2; ++k) {
            const uint32_t s = p.str(k ? 0xfec : 0xfeb);
            const int32_t y = p.YH() - (k ? 0x75 : 0x7d);
            p.text(s, p.centre(s, p.X() + 0xb, 0x4e), y, 1);
        }
        p.itoa(0x8c, (int32_t)e.H(kStatBase, unit, 4, 0));
        p.font(1);
        p.w2u(0x78, p.Nb(0x8c), 9);
        const int32_t y = p.YH() - 0x78;
        p.text(p.A(0x78), p.centre(p.A(0x78), p.X() + 0x5c, 0x24), y, 0);
    }
    // the four +stat buttons (.data 0xDD810, stride 0xE: x, y, pressed)
    if (e.H(kStatBase, unit, 4, 0)) {
        for (uint32_t k = 0; k < 4; ++k) {
            const uint32_t b = 0xdd810 + 0xe * k;
            const uint32_t h0 = p.G(0x11c304);
            const int32_t x = (int32_t)p.G(b) + p.X();
            const int32_t yb = p.YH() + (int32_t)p.G(b + 4) - 0x1e0;
            p.cel(h0, 0, x + 5, yb + 5);
            p.cel(p.G(0x11c300), p.G(b + 8) ? 1u : 0u, x + 8, yb + 1);
        }
    }
    p.font(6);
    for (const Lbl& l : kLabels) {
        p.ctor(0x218, 100);
        // retail loads only CX (`mov cx, [id]`): ECX's high half is the Unicode ctor loop's leftover stack pointer
        const uint32_t s = p.str((p.A(0x218 + 2 * 99) & 0xffff0000u) | l.id);
        {   // strncpy(buf, s, 0x63): copy to NUL or n chars, zero-pad to n
            char16_t* d = p.Wb(0x218); int32_t i = 0;
            for (; i < 0x63; ++i) { const char16_t c = (char16_t)(e.R(s + 2 * (uint32_t)i) & 0xffff); d[i] = c; if (!c) break; }
            for (; i < 0x63; ++i) d[i] = 0;
        }
        int32_t x0 = l.x0 + p.X();
        const int32_t x1 = l.x1 + p.X();
        p.D(0x18) = (uint32_t)(l.y + p.YH() - 0x1e0);
        p.Wb(0x218)[99] = 0;
        int32_t len = 0; while (p.Wb(0x218)[len]) ++len;
        int32_t i = 0; bool nl = false;
        for (char16_t* q = p.Wb(0x218); *q; ++q, ++i) if (*q == 0xa) { nl = true; break; }
        if (nl) {
            p.Wb(0x218)[i] = 0;
            if (i < len) {
                p.D(0x20) = p.A(0x218 + 2 * (uint32_t)(i + 1));
                const int32_t span = x1 - x0 + 1;
                p.text(p.A(0x218), p.centre(p.A(0x218), x0, span), (int32_t)p.D(0x18) - 4, 0);
                x0 = p.centre(p.D(0x20), x0, span);
                p.text(p.D(0x20), x0, (int32_t)p.D(0x18) + 4, 0);
                continue;
            }
        }
        p.text(p.A(0x218), p.centre(p.A(0x218), x0, x1 - x0 + 1), (int32_t)p.D(0x18), 0);
    }
    // a one-time language adjustment of two layout words
    if (!p.G8(0x1234a0)) {
        if ((int32_t)e.H(kLang) >= 6 && (int32_t)e.H(kLang) <= 9) {
            p.SetG(0xf52ec, p.G(0xf52ec) - 1);
            p.SetG(0xf5334, p.G(0xf5334) - 1);
        }
        p.SetG(0x1234a0, (p.G(0x1234a0) & ~0xffu) | 1);
    }
    // the two weapon slots
    if (unit) {
        e.H(0x2fd30, unit, e.H(0xc5bc, unit), 0);
        e.H(0x2fd30, unit, e.H(0xc5ce, unit), 6);
    }
    p.font(0);
    const uint32_t v1 = e.H(0xbcb90, e.H(0x2e350, unit, 1));
    const uint32_t v0 = e.H(0xbcb90, e.H(0x2e350, unit, 0));
    const uint32_t r = e.H(0xbc520);
    e.H(0xbc8f0, (uint32_t)my, v1, v0, 0, 0, (uint32_t)mx);
    e.H(0xbc760, (uint32_t)mx, (uint32_t)my, 0, 0, 0, r);
    p.font(1);
    // the buy/sell button cel, loaded once
    uint32_t bs = p.G(0x11b9c4);
    if (!bs) {
        const char* a = "DATA\\GLOBAL\\ui\\panel\\buysellbtn";
        memcpy(p.Nb(0x218), a, strlen(a) + 1);
        bs = e.H(0x2b420, 0, 0, 0, 0, 0, p.A(0x218));
        p.SetG(0x11b9c4, bs);
    }
    p.cel(bs, 0xa + (p.G(0x11bbc4) ? 1u : 0u), p.X() + 0x80, p.YH() - 0x3c);
    // class and character name
    p.ctor(0x118, 0x80);
    if (unit && e.R(unit) == 0) {
        const int32_t c = (int32_t)e.R(unit + 4);
        uint32_t rec = 0;
        if (c >= 0 && c < (int32_t)e.R(e.dt + 0xbc8)) rec = e.R(e.dt + 0xbc4) + (uint32_t)c * 0xc4;
        const int32_t y = p.YH() - 0x1c7;
        p.D(0x14) = (uint32_t)y;
        p.text(rec, p.centre(rec, p.X() + 0xc1, 0x76), y, 0);
        const uint32_t pd = e.H(0x9d630, 0, 0, 0, 0, 0, 0, 0, e.R(unit + 0xc));
        uint32_t name = pd ? pd + 0x66 : e.H(0xc7de, unit);
        if (!pd && !name) return;   // retail: Fog 10265/10024 then exit(-1) (not vectored)
        int32_t n8 = 0; while (e.R8(name + (uint32_t)n8)) ++n8;
        const uint32_t n = nwidth(e, name, n8) + 1;
        if (n > 0xb) p.font(n >= 0xe ? 6 : 0);
        p.w2uA(0x118, name, 0x80);
        const int32_t y2 = p.YH() - 0x1c7;
        p.text(p.A(0x118), p.centre(p.A(0x118), p.X() + 0xd, 0x94), y2, 0);
        p.font(1);
    }
    // the value rows
    for (uint32_t ri = 0; ri < 18; ++ri) {
        p.D(0x18) = ri;
        const Row& w = kRows[ri];
        uint32_t big = 0;
        int32_t v = (int32_t)e.H(kStat, unit, w.stat, 0);
        const int32_t base = (int32_t)e.H(kStatBase, unit, w.stat, 0);
        uint32_t& col = p.D(0x10);
        const auto cmpc = [&] { if (v > base) col = 3; else if (v < base) col = 1; };
        bool numDraw = false;
        switch (w.stat) {
        case 6: {
            v >>= 8;
            const uint32_t pl = p.G(0x11bbfc);
            if (pl && e.R(pl) == 0 && e.R(pl + 0x10) != 0x11 && v >= 0 && v <= 1) v = 1;
            p.itoa(0x98, v); big = 1; break;
        }
        case 7: case 9: case 11: cmpc(); v >>= 8; p.itoa(0x98, v); big = 1; break;
        case 8: case 10: v >>= 8; p.itoa(0x98, v); big = 1; break;
        case 13: numDraw = true; break;
        case 30: {
            const uint32_t lvl = e.H(kStatBase, unit, 0xc, 0);
            p.D(0x14) = lvl;
            const uint32_t cls = unit ? e.R(unit + 4) : 0xffffffffu;
            if (lvl != e.H(0xc28c, cls)) v = (int32_t)e.H(0xc24a, cls, lvl);
            numDraw = true; break;
        }
        case 31: {
            v = (int32_t)e.H(0xc2a4, unit);
            if (e.H(0xc280, unit)) col = 3;
            const uint32_t q = e.R(unit + 0x60);
            if (q && e.H(0xc274, q, 0) && e.H(0xc1fc, unit, 0x65)) col = 3;
            if (e.H(0xc23e, unit)) col = 1;
            p.itoa(0x98, v);
            if (v >= 1000 || e.H(kLang) == 6) big = 1;
            break;
        }
        case 39: case 41: case 43: case 45: {
            p.D(0x14) = e.H(0xc26e, p.G8(0x11c390));
            uint32_t hi = 0, lo = 0, mxs = 0;
            switch (w.stat) {
            case 39: hi = 0xc2b0; lo = 0xc298; mxs = 0x28; break;
            case 41: hi = 0xc268; lo = 0xc238; mxs = 0x2a; break;
            case 43: hi = 0xc256; lo = 0xc27a; mxs = 0x2c; break;
            default: hi = 0xc29e; lo = 0xc25c; mxs = 0x2e; break;
            }
            if (e.H(hi, unit)) col = 3;
            if (e.H(lo, unit)) col = 1;
            int32_t cap = (int32_t)e.H(kStat, unit, mxs, 0);
            if (p.G(0x119854)) v = (int32_t)((uint32_t)v + e.R(p.D(0x14)));
            else if (p.G8(0x11c390) == 1) v = (int32_t)((uint32_t)v - 0x14);
            else if (p.G8(0x11c390) == 2) v = (int32_t)((uint32_t)v - 0x32);
            cap = (int32_t)((uint32_t)cap + 0x4b); if (cap >= 0x5f) cap = 0x5f;
            if (v <= -100) v = -100;
            if (v >= cap) {
                v = cap;
                if (col != 3) { col = 4; p.itoa(0x98, v); break; }
            }
            if (v < 0) col = 1;
            p.itoa(0x98, v);
            break;
        }
        default: cmpc(); p.itoa(0x98, v); break;
        }
        const int32_t X = p.X();
        const int32_t x0 = w.x0 + X, span = w.x1 - x0 + X + 1, y = w.y + p.YH() - 0x1e0;
        if (numDraw) {
            e.H(kNum, p.A(0x118), (uint32_t)v, 0x80);
            p.text(p.A(0x118), p.centre(p.A(0x118), x0, span), y, 0);
            col = 0;
            continue;
        }
        p.w2u(0x118, p.Nb(0x98), 0x80);
        p.Wb(0x118)[127] = 0;
        uint32_t ebp = 0xe;
        if (big) {
            if (v < 1000) {
                e.H(kMeasure, p.A(0x74), 0, 0, 0, 0, 0, p.A(0x118), p.A(0x28));
                if ((int32_t)p.D(0x28) >= w.x1 - w.x0) ebp = p.font(0);
            } else {
                ebp = p.font(0);
            }
        }
        const int32_t X2 = p.X();
        const int32_t x0b = w.x0 + X2, span2 = w.x1 - x0b + X2 + 1, y2 = w.y + p.YH() - 0x1e0;
        p.text(p.A(0x118), p.centre(p.A(0x118), x0b, span2), y2, col);
        col = 0;
        if (ebp != 0xe) p.font(ebp);
    }
}
