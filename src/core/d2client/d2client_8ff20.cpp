// 1.13c D2Client+0x8FF20 item tooltip damage line: stdcall (item, out), ret 8.
// player = the value of the retail dword 0x11BBFC. Every string operation is a call: the D2Lang Unicode imports by
// their IAT slots (strcpy 0xCEAB8, strcat 0xCEABC, win2Unicode 0xCEAC0; the default-constructor loops are not
// observable), the string lookup thunk 0xD1EC (ECX = id), the "%i" formatter thunk 0xBEA2 (cdecl buf, 0x100, fmt, v),
// the colour prefix 0xBF5F0 (EDI = buffer, stack colour) and damageRange 0x8DAD0 (EAX = item, stack player, lo stat,
// hi stat, &min, &max, &flag).
#include "cl_env.h"
namespace {
constexpr uint32_t kCpy = 0xCEAB8, kCat = 0xCEABC, kW2u = 0xCEAC0, kFmt = 0xB2000000u | 0xD34C4;
struct F {
    const cl::Env& e;
    uint32_t sp, s1, s2, num;   // a stack scratch pointer, the two separator strings, the 9-/5-char number buffer
    uint32_t str(uint32_t id) const { return e.H(0xd1ec, 0, 0, 0, 0, 0, 0, id); }
    void cpy(uint32_t d, uint32_t s) const { e.H(kCpy, 0, 0, 0, 0, 0, 0, d, s); }
    void cat(uint32_t d, uint32_t s) const { e.H(kCat, 0, 0, 0, 0, 0, 0, d, s); }
    void colour(uint32_t buf, uint32_t c) const { e.Ed(buf); e.H(0xbf5f0, c); }
    // sprintf(scratch, 0x100, "%i", v) then win2Unicode(num, scratch, n)
    void number(int32_t v, uint32_t n) const { e.H(0xbea2, sp, 0x100, kFmt, (uint32_t)v); e.H(kW2u, n, 0, 0, 0, 0, 0, num, sp); }
    void range(uint32_t item, uint32_t pl, uint32_t lo, uint32_t hi, int32_t* m) const {
        e.Xs((uint32_t)(uintptr_t)&m[2]);
        e.H(0x8dad0, pl, lo, hi, (uint32_t)(uintptr_t)&m[0], (uint32_t)(uintptr_t)&m[1], item);
    }
};
}  // namespace
extern "C" void d2c_8ff20(uint32_t item, uint32_t out, uint32_t player, const cl::Env& e) {
    uint16_t A[0x100], B[0x100], N[0x100]; char S[0x100];
    const uint32_t a = (uint32_t)(uintptr_t)A, b = (uint32_t)(uintptr_t)B, n = (uint32_t)(uintptr_t)N;
    F f{ e, (uint32_t)(uintptr_t)S, 0, 0, n };
    f.s1 = f.str(0xf9b);
    f.s2 = f.str(0xf9e);
    int32_t m[3] = { 0, 0, 0 };   // min, max, flag/colour (the retail locals S+0x14, S+0x18, S+0x10)
    if (e.H(0xc1cc, item, 0x26)) {
        const uint32_t u = e.H(0xc736, item);
        m[2] = 0;
        m[0] = (int32_t)e.H(0xc880, 0, player, u, 1);
        m[1] = (int32_t)e.H(0xc96a, 0, player, u, 1);
        f.cpy(a, f.str(0xd8b));
        f.colour(a, 0);
        switch ((e.H(0xc868, u) & 0xff)) {
        case 1: m[2] = 1; break;
        case 2: m[2] = 4; break;
        case 4: m[2] = 3; break;
        case 5: {
            m[2] = 2;
            int32_t q = (int32_t)e.H(0xc832, 0, player, u, 1) / 25;
            if (q <= 0) q = 1;
            m[0] /= q; m[1] /= q;
            break;
        }
        default: break;
        }
        m[0] = (int32_t)(m[0] + e.H(0xc8e0, 0, player, u, 1)) >> 8;
        int32_t hi = (int32_t)(e.H(0xc934, 0, player, u, 1) + (uint32_t)m[1]) >> 8;
        if (hi <= m[0]) hi = m[0];
        f.number(m[0], 9);
        f.colour(n, (uint32_t)m[2]);
        f.cat(a, f.s1); f.cat(a, n);
        if (m[0] != hi) {
            f.cat(a, f.s1);
            f.cat(a, f.str(0xd88));
            f.number(hi, 9);
            f.colour(n, (uint32_t)m[2]);
            f.cat(a, f.s1); f.cat(a, n);
        }
        f.cat(a, f.s2);
        f.cpy(out, a);
        return;
    }
    m[2] = 0;
    if (e.H(0xc964, player, item)) {
        // both lines: two-hand (0x17/0x18) into B, one-hand (0x15/0x16) into A; out = B then A
        const uint32_t ids[2][3] = { { 0x17, 0x18, 0xd8a }, { 0x15, 0x16, 0xd89 } };
        const uint32_t bufs[2] = { b, a };
        for (int k = 0; k < 2; ++k) {
            const uint32_t d = bufs[k];
            f.range(item, player, ids[k][0], ids[k][1], m);
            f.cpy(d, f.str(ids[k][2]));
            f.cat(d, f.s1);
            f.number(m[0], 5);
            if (m[2]) f.colour(n, 3);
            f.cat(d, n); f.cat(d, f.s1);
            f.cat(d, f.str(0xd88));
            f.number(m[1], 5);
            f.cat(d, f.s1); f.cat(d, n); f.cat(d, f.s2);
        }
        f.colour(b, 0);
        f.colour(a, 0);
        f.cpy(out, b);
        f.cat(out, a);
    } else {
        const bool two = e.H(0xc87a, item) != 0;
        f.range(item, player, two ? 0x17 : 0x15, two ? 0x18 : 0x16, m);
        const int32_t lo1 = (int32_t)((uint32_t)m[0] + 1u);
        if (m[1] <= lo1) m[1] = lo1;
        f.cpy(out, f.str(two ? 0xd8a : 0xd89));
        f.number(m[0], 9);
        f.cat(out, f.s1);
        if (m[2]) f.colour(n, 3);
        f.cat(out, n); f.cat(out, f.s1);
        f.cat(out, f.str(0xd88));
        f.number(m[1], 9);
        f.cat(out, f.s1); f.cat(out, n); f.cat(out, f.s2);
    }
    m[2] = 0;
    if (e.H(0xc98e, item, 0x12, 0) || e.H(0xc98e, item, 0x11, 0) || e.H(0xc98e, item, 0x9f, 0) || e.H(0xc98e, item, 0xa0, 0))
        m[2] = 1;
    if (!e.H(0xc76c, item)) return;
    f.cpy(a, f.str(0xd8b));
    f.colour(a, 0);
    f.range(item, player, 0x9f, 0xa0, m);
    f.number(m[0], 9);
    const uint32_t c = m[2] ? 3u : 0u;
    f.colour(n, c);
    f.cat(a, f.s1); f.cat(a, n); f.cat(a, f.s1);
    f.cat(a, f.str(0xd88));
    f.number(m[1], 9);
    f.colour(n, c);
    f.cat(a, f.s1); f.cat(a, n); f.cat(a, f.s2);
    f.cat(out, a);
}
