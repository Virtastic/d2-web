// 1.13c D2Client+0x8EA20 (0x720 B, EBX = unit, no stack words, bare ret): the twelve number rows of the character
// stats panel.
// Rows come from the table at 0xE1210 (stride 0x14: x0, x1, y, width threshold, stat id). Per row the value is D2Common
// 10973 (unit, stat, 0) against 10587 (the base): higher draws colour 3, lower colour 1. x0/x1 move by 0x11B9A0 and y by
// -0x11B9A4 - 2. Stat 6 (life) is "cur / max" through Storm 578 and toUnicode; stat 0x15 the damage range; stat 0x1F the
// 11160 value; the four resistances (0x27/0x29/0x2B/0x2D) take the difficulty penalty (0x11C390; with 0x119854 set, the
// word D2Common 10237 points at) and clamp to [-100, 75 + max-res]; any other stat is the number (D2Lang 10002). Then
// font 0, D2Win 10177 width (font 6 and y+1 above the threshold) and D2Win 10150 right-aligned at x1 (x0 == 0),
// left-aligned at x0 (x1 == 0) or centred between them. The panel font is restored at the end.
// Every import and callee goes through e.H with the registers and stack words retail passes; the Unicode ctor loop runs
// real in retail and is not a call here.
#include "cl_env.h"
namespace {
constexpr uint32_t G(uint32_t rva) { return 0x6FAB0000u + rva; }
constexpr uint32_t kToU = 0xCEAC4;
inline uint32_t A(const volatile void* p) { return (uint32_t)(uintptr_t)p; }
inline int32_t mul(int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)b); }
inline int32_t st(const cl::Env& e, uint32_t u, uint32_t id) { return (int32_t)e.H(0xC1C6, u, id, 0); }
}  // namespace
// 1.13c D2Client+0x8EA20
extern "C" void d2c_8ea20(uint32_t unit, const cl::Env& e) {
    volatile uint16_t buf[0x80];
    volatile char cb[0x80];
    volatile int32_t tw = 0, th = 0;
    if (unit == 0) return;
    const uint32_t oldFont = e.H(0xD3A8, 0, 0, 0, 0, 0, 0, 0);
    for (uint32_t i = 0; i < 12; ++i) {
        for (int k = 0; k < 0x80; ++k) buf[k] = 0;
        const uint32_t row = G(0xE1210 + i * 0x14);
        uint32_t col = 0;
        const uint32_t stat = e.R(row + 0x10);
        int32_t v = st(e, unit, stat);
        const int32_t base = (int32_t)e.H(0xC250, unit, stat, 0);
        if (v > base) col = 3;
        else if (v < base) col = 1;
        const uint32_t xo = e.R(G(0x11B9A0));
        const int32_t x0 = (int32_t)(e.R(row) + xo), x1 = (int32_t)(e.R(row + 4) + xo);
        int32_t y = (int32_t)(e.R(row + 8) - e.R(G(0x11B9A4)) - 2);
        auto num = [&](int32_t n) { e.H(0xD1F8, A(buf), (uint32_t)n, 0x80); };
        auto fmt = [&](uint32_t f, int32_t a, int32_t b) {
            e.H(0xBEA2, A(cb), 0x80, 0xB2000000u | f, (uint32_t)a, (uint32_t)b);
            e.H(kToU, 0x80, 0, 0, 0, 0, 0, A(buf), A(cb));
        };
        if (stat == 6) {
            const int32_t mx = (int32_t)e.H(0xC196, unit) >> 8;
            int32_t cur = mul((int32_t)e.H(0x21580, 0, 0, 0, 0, 0, 0, e.R(unit + 0xC)), mx) / 100;
            if (e.H(0xA5440, 0, 0, 0, 0, 0, unit) == 0 && cur < 0) cur = 1;
            fmt(0xD45C8, cur, mx);
            if (((int32_t)e.H(0xC250, unit, 7, 0) >> 8) < mx) col = 3;
        } else if (stat == 0x15) {
            int32_t edi = st(e, unit, 0x19);
            const int32_t v12 = st(e, unit, 0x12), v11 = st(e, unit, 0x11);
            if (v12 + edi > 0) col = 3;
            else if (v12 + edi < 0) col = 1;
            uint32_t w = 0;
            int32_t ebp = 0;
            const uint32_t inv = e.R(unit + 0x60);
            if (inv) w = e.H(0xC262, inv, 4);
            int32_t mx = st(e, unit, 0x16), mn = v;
            if (w) {
                if (e.H(0xC87A, w)) {
                    mn = v - st(e, w, 0x15);
                    mx = mx - st(e, w, 0x16);
                    mn += st(e, unit, 0x17);
                    mx += st(e, unit, 0x18);
                }
                int32_t p = (int16_t)e.H(0xC940, w);
                if (p) edi += mul(st(e, unit, 0), p) / 100;
                p = (int16_t)e.H(0xC8CE, w);
                if (p) edi += mul(st(e, unit, 2), p) / 100;
                edi += (int32_t)e.H(0xC922, unit, w, 0, 1);
                ebp = (int32_t)e.H(0xC454, w, 0x6F, 0);
            } else {
                edi += st(e, unit, e.R(unit + 4) == 0x10F ? 2 : 0);
            }
            if (edi <= -90) edi = -90;
            int32_t lo = mul(v12 + edi + 100, mn) / 100 + ebp;
            int32_t hi = mul(v11 + edi + 100, mx) / 100 + ebp;
            static const uint32_t ex[4][3] = {{0x30, 0x31, 1}, {0x32, 0x33, 9}, {0x34, 0x35, 3}, {0x36, 0x37, 3}};
            for (const auto& t : ex) {
                const int32_t a = st(e, unit, t[0]), b = st(e, unit, t[1]);
                if (a != 0 || b != 0) col = t[2];
                hi += b;
                lo += a;
            }
            const int32_t a = st(e, unit, 0x39), b = st(e, unit, 0x3A);
            if (a != 0 || b != 0) {
                int32_t wv = st(e, unit, 0x65);
                if (wv <= 0) {
                    int32_t d = st(e, unit, 0x146);
                    if (d < 1) d = 1;
                    wv = st(e, unit, 0x3B) / d;
                }
                lo += mul(wv, a) >> 8;
                col = 2;
                hi += mul(wv, b) >> 8;
            }
            fmt(0xD45C0, lo, hi);
        } else if (stat == 0x1F) {
            const uint32_t n = e.H(0xC2A4, unit);
            if (e.H(0xC280, unit)) col = 3;
            num((int32_t)n);
        } else if (stat == 0x27 || stat == 0x29 || stat == 0x2B || stat == 0x2D) {
            const uint32_t dp = e.H(0xC26E, e.R8(G(0x11C390)) & 0xFF);
            static const uint32_t up[4] = {0xC2B0, 0xC268, 0xC256, 0xC29E}, dn[4] = {0xC298, 0xC238, 0xC27A, 0xC25C};
            const uint32_t s = (stat - 0x27) >> 1;
            if (e.H(up[s], unit)) col = 3;
            if (e.H(dn[s], unit)) col = 1;
            const int32_t mxr = st(e, unit, stat + 1);
            if (e.R(G(0x119854)) != 0) v += (int32_t)e.R(dp);
            else {
                const uint32_t d = e.R8(G(0x11C390)) & 0xFF;
                if (d == 1) v -= 20;
                else if (d == 2) v -= 50;
            }
            const int32_t cap = mxr + 75 < 95 ? mxr + 75 : 95;
            const int32_t val = v > -100 ? v : -100;
            if (val >= cap) { col = 4; num(cap); }
            else { if (val < 0) col = 1; num(val); }
        } else {
            num(v);
        }
        e.H(0xD3A8, 0, 0, 0, 0, 0, 0, 0);
        tw = 0; th = 0;
        e.H(0xD3B4, A(&th), 0, 0, 0, 0, 0, A(buf), A(&tw));
        const int32_t thr = (int32_t)e.R(row + 0xC);
        if (thr != 0 && tw > thr) {
            e.H(0xD3A8, 0, 0, 0, 0, 0, 0, 6);
            ++y;
        }
        int32_t x;
        if (e.R(row) == 0) x = x1 - (int32_t)e.H(0xD35A, 0, 0, 0, 0, 0, 0, A(buf));
        else if (e.R(row + 4) == 0) x = x0;
        else {
            const int32_t span = x1 - x0 + 1, w = (int32_t)e.H(0xD35A, 0, 0, 0, 0, 0, 0, A(buf));
            x = w < span ? x0 + ((span - w) >> 1) : x0;
        }
        e.H(0xD37E, (uint32_t)y, col, 0, 0, 0, 0, A(buf), (uint32_t)x);
    }
    e.H(0xD3A8, 0, 0, 0, 0, 0, 0, oldFont);
}
