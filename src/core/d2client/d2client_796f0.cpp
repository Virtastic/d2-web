// 1.13c D2Client+0x796F0 (1411 B, one stack arg = mouse message record P, ret 4): left-button-down handler of the belt / HUD panel row.
// x = u16[P+0xC], y = u16[P+0xE]. Early outs: 0x26320(EAX = P) non-zero, no player (0x11BBFC), y > H-0x30 (P[0x18] untouched).
// All other paths set P[0x18] = 1 and P[0x1C] = 0 and call 0xBE84(P) except the last (x outside the row: P[0x18] = 0).
// The globals sit in the pinned array e.gl (offsets below); the player pointer is e.self and gdwBitMasks is e.bm.
#include "cl_env.h"
namespace {
enum : uint32_t {
    G_CBFC8 = 0, G_B9A0 = 4, G_B9A4 = 8, G_W = 12, G_H = 16, G_924F8 = 20, G_925F0 = 24, G_AAD90 = 28, G_D5D0 = 32,
    G_AAD88 = 36, G_AADD0 = 40, G_AADBC = 44, G_AADC0 = 48, G_AAE10 = 52, G_AAE14 = 56, G_AADD8 = 60,
    G_A9E14 = 64, G_A9E18 = 68, G_C3C4 = 72, G_C414 = 76, G_C418 = 80, G_C3DC = 84, G_C3E0 = 88, G_C3F8 = 92, G_C3FC = 96,
    G_BFB4 = 100, G_C3D38 = 104 /* 8 dwords */, G_C3B38 = 136 /* 8 dwords */
};
}  // namespace
// 1.13c D2Client+0x796F0
extern "C" void d2c_796f0(uint32_t P, const cl::Env& e) {
    const uint32_t gl = e.gl;
    const auto g = [&](uint32_t o) { return (int32_t)e.R(gl + o); };
    const auto sg = [&](uint32_t o, int32_t v) { e.W(gl + o, (uint32_t)v); };
    const auto done = [&](uint32_t v) { e.W(P + 0x18, 1); e.W(P + 0x1c, 0); e.H(0xbe84, P); (void)v; };
    const int32_t X = (int32_t)(e.R(P + 0xc) & 0xffff), Y = (int32_t)((e.R(P + 0xc) >> 16) & 0xffff);
    if (e.H(0x26320, 0, 0, 0, 0, 0, P)) return;
    const uint32_t self = e.self;
    if (!self) return;
    const int32_t H = g(G_H);
    const uint32_t vec = e.R(self + 4);
    if (Y > H - 0x30) return;
    if (g(G_CBFC8) == 0) {
        sg(G_C3DC, 0); sg(G_C3E0, 0); sg(G_C3F8, 0); sg(G_C3FC, 0);
        done(0);
        return;
    }
    const int32_t L = g(12) - g(G_B9A0);   // W - cb9a0
    const auto cnt = [&]() { return (int32_t)e.H(0xcb38, vec); };
    // slot lookup shared by both loops: 0 when 15b0 / 17f0 give nothing; returns the 17f0 record in rec, the 15b0 one in def
    const auto look = [&](int32_t idx, uint32_t& def, uint32_t& rec) {
        const uint32_t s = e.H(0xcb3e, vec, (uint32_t)idx);
        def = e.H(0x15b0, 0, 0, 0, 0, 0, s);
        rec = 0;
        if (!def) return false;
        rec = e.H(0x17f0, 0, 0, 0, 0, 0, s);
        return rec != 0;
    };
    const auto near = [&](uint32_t rec) {
        const uint32_t r3 = e.R8(rec + 3);   // EAX above AL is the record pointer's residue: the capture logs AL only
        const int32_t a = (int32_t)e.H(0x77ca0, 0, 0, 0, 0, 0, r3);
        if (!(Y > a - 0x30 && Y < a)) return false;
        const uint32_t r4 = ((uint32_t)a & ~0xffu) | e.R8(rec + 4);
        const int32_t b = (int32_t)e.H(0x77c50, 0, 0, 0, 0, 0, r4);
        return X > b && X < b + 0x30;
    };
    if (X > L - 0x140 && X < L - 0x58) {
        if ((int32_t)e.H(0xc250, self, 5, 0) <= 0) {
            int32_t idx = 0;
            if (cnt() > 0) {
                do {
                    uint32_t def, rec;
                    if (look(idx, def, rec)) {
                        bool ok = (int32_t)g(G_924F8) == (int32_t)(int8_t)e.R8(rec + 2) && near(rec) && e.R(gl + G_C3D38 + 4 * (uint32_t)idx) == 0;
                        if (ok) ok = (e.R8(e.bm + 0x10) & e.R8(def + 4)) == 0;
                        if (ok) {
                            e.H(0x14910, 0, 0, 0, 0, 0, ((uint32_t)idx & ~0xffu) | 0x3c, (uint32_t)(int32_t)(int16_t)(e.R(def) & 0xffff), 0xffffffffu);
                            const uint32_t old = e.R(gl + G_AAD90);
                            e.H(0xbf8c);
                            if (g(G_D5D0) != 0x26) {
                                e.H(0xbf92, 0x139b);
                                e.H(0xbf80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                                e.H(0x3664, 0xffffffffu);
                                return;
                            }
                            e.W(gl + G_AAD90, 0);
                            if (old) {
                                const uint32_t t = e.H(0xd0b4);
                                e.H(0x3cc60, t, 0xB20D0604u, 0, 0, 0, 2);
                                e.H(0xbe2e0);
                                e.H(0x77da0);
                            }
                            if (!g(G_AAD88) && !g(G_AADD0) && !g(G_AADBC) && !g(G_AADC0) && !g(G_AAE10) && !g(G_AAE14) && !g(G_AADD8)) {
                                sg(G_C418, 0); sg(G_C414, 0);
                                e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
                                e.H(0x8a4d0, 0, 0, 0, 0, 0, (uint32_t)g(G_A9E18), (uint32_t)g(G_A9E14), 0, 0, (uint32_t)g(G_C3C4));
                            } else {
                                const int32_t W = g(12);
                                const int32_t s = (W + ((W >> 31) & 3)) >> 2;
                                sg(G_C418, s); sg(G_C414, 2);
                                e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)s);
                                const int32_t c = g(G_A9E14);
                                const int32_t t = (c + ((c >> 31) & 3)) >> 2;
                                e.H(0x8a4d0, (uint32_t)t, 0, 0, 0, 0, (uint32_t)g(G_A9E18), (uint32_t)(c + t), 0, 0, (uint32_t)g(G_C3C4));
                            }
                        } else done(0);
                    }
                    ++idx;
                } while (idx < cnt());
            }
            done(0);
        }
    }
    const int32_t H2 = g(G_H);
    if (X >= L - 0x58) {
        if (X <= L) {
            const int32_t old = g(G_924F8), t = g(G_B9A4) + H2;
            if (Y > t - 0x174 && Y < t - 0x109) sg(G_924F8, 3);
            if (Y > t - 0x108 && Y < t - 0x9d) sg(G_924F8, 2);
            if (Y > t - 0x9c && Y < H2 - 0x31) sg(G_924F8, 1);
            if (g(G_924F8) != old) e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 6);
        }
        done(0);
    }
    const int32_t m = g(G_925F0);
    if (m == 1 || m == 2 || m == 3) {
        const int32_t ax = m == 1 ? L - 0x95 : m == 2 ? L - 0xdc : L - 0x131;
        if (e.H(0x779f0, 0x22, 0, 0, 0, 0, (uint32_t)ax, 0, (uint32_t)(g(G_B9A4) + H2 - 0x3f), 0, 0x22)) {
            e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
            sg(G_BFB4, 1);
        }
    }
    if (X > L - 0x140 && X < L - 0x58) {
        if ((int32_t)e.H(0xc250, self, 5, 0) <= 0) { done(0); return; }
        int32_t idx = 0;
        if (cnt() > 0) {
            do {
                uint32_t def, rec;
                if (look(idx, def, rec) && (int32_t)g(G_924F8) == (int32_t)(int8_t)e.R8(rec + 2) && near(rec) && e.R(gl + G_C3D38 + 4 * (uint32_t)idx) == 0) {
                    e.W(gl + G_C3D38 + 4 * (uint32_t)idx, 1);
                    e.W(gl + G_C3B38 + 4 * (uint32_t)idx, 3);
                    e.H(0x16360, 0, 0, 0, 0, 0, 0, 0, 0, 0, P);
                    e.W(P + 0x18, 1); e.W(P + 0x1c, 0);
                    e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 5);
                    e.H(0xbe84, P);
                    return;
                }
                ++idx;
            } while (idx < cnt());
        }
        done(0);
        return;
    }
    e.W(P + 0x18, 0);
}
