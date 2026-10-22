// D2Client sub_65760: per-frame panel-state reconcile + scroll-offset update. Env-seam port (callees/globals injected).
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cUiFrameEnv {
    u32 base;                                   // D2Client image base (0x6FAB0000 in retail)
    u32 (*rd32)(u32 addr); void (*wr32)(u32 addr, u32 v);
    u32 (*rd8)(u32 addr);  void (*wr8)(u32 addr, u32 v);
    i32 (*ord10227)(); void (*ord10006)(); u32 (*ord10048)();
    i32 (*f71a80)(); void (*f721f0)(); void (*f3bbf0)(); void (*f11f80)();
    void (*f3a4d0)(i32 a, i32 b, i32 c);        // D2Client+0x8A4D0
    void (*f72790)(u32 i, i32 a, i32 b);
    void (*f47b20)(); i32 (*f55440)(); void (*fa6210)(); void (*f14a30)(); void (*f14660)();
    void (*f3ccc0)(u32 ord10048, u32 guestAddr); void (*f3cc60)(u32 ord10048, u32 guestAddr);
    u32 (*tick)();
    void (*fatal)(i32 line);                    // Ordinal_10265/10024 + _exit(-1)
};

extern "C" {
// 1.13c D2Client+0x65760
EMSCRIPTEN_KEEPALIVE void d2c_65760(const D2cUiFrameEnv* e, i32 doLoop, i32 mode) {
    const u32 B = e->base;
    auto G = [&](u32 rva) { return e->rd32(B + rva); };
    auto S = [&](u32 rva, u32 v) { e->wr32(B + rva, v); };
    const u32 ARR = 0xFAD80;
    if (doLoop) {
        for (u32 i = 0, off = 0; off < 0x1c8; i++, off += 0xc) {
            S(0xE2900 + off, 0);
            if (G(0xE28f8 + off) == 0) continue;
            if (G(0xE28fc + off) != 0) S(0xE2900 + off, G(ARR + 4 * i));
            i32 old = (i32)G(ARR + 4 * i);
            if (old == 0) continue;
            bool a = e->ord10227() == 0 || G(0x119854) == 0;
            if ((a && (off == 0x1a4 || off == 0x1b0 || off == 0x1bc)) || e->f71a80() == 0) continue;
            S(ARR + 4 * i, 0);
            e->f721f0();
            i32 x = 0, y = 0;
            auto q = [](i32 v) { return (v + ((v >> 31) & 3)) >> 2; };
            auto flat = [&]() { S(0x11c418, 0); S(0x11c414, 0); e->ord10006(); y = 0; x = (i32)G(0xf9e14); };
            switch (i) {
            case 1: e->f3bbf0(); /* fallthrough */
            case 4:
                if (!G(ARR + 8) && !G(ARR + 0x50) && !G(ARR + 0x3c) && !G(ARR + 0x40) && !G(ARR + 0x90) &&
                    !G(ARR + 0x94) && !G(ARR + 0x58)) flat();
                else {
                    S(0x11c418, q((i32)G(0xdbc48))); S(0x11c414, 2); e->ord10006();
                    y = q((i32)G(0xf9e14)); x = (i32)G(0xf9e14) + y;
                }
                break;
            case 2: case 0xf: case 0x10: case 0x14: case 0x16: case 0x24: case 0x25:
                if (!G(ARR + 4) && !G(ARR + 0x10)) { flat(); break; }
                S(0x11c418, (u32)-q((i32)G(0xdbc48))); S(0x11c414, 1); e->ord10006();
                y = q((i32)G(0xf9e14)); x = (i32)G(0xf9e14) - y; y = -y;
                break;
            case 0xb: e->f11f80(); continue;
            case 0xc: case 0xe: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x20:
                flat(); break;
            default: continue;
            }
            e->f3a4d0(x, 0, y);
        }
    }
    i32 prev = (i32)G(0xfada4);
    e->ord10227();
    i32 n = (i32)G(0xDD5D0);
    if (n != 0x26) { e->fatal(0x139b); return; }
    u32 t = e->tick(), pp = G(0x11bbfc);
    if (G(0xfad50) <= t && pp) {
        if (e->rd32(pp) == 0 && e->rd32(pp + 16) == 0x11) {
            e->fa6210(); e->f14a30();
            u32 f = G(0x11b980) + 0x1eb;
            if (e->rd8(f) & 4) {
                e->f14660(); e->wr8(f, e->rd8(f) | 8);
                S(0x11c3a0, 0); S(0x11c39c, 0); S(0xf8c9c, 0); S(0x11c3a4, 1);
            }
        } else if (G(0x113a68) == 0) {
            bool bail = false;
            for (i32 u = 0; u < n && !bail; u++) {
                if (G(ARR + 4 * u) != 1) continue;
                switch (e->rd32(G(0xDD538 + 4 * u) + 0x24)) {
                case 0: break;
                case 1: e->f72790(u, 1, 0); pp = G(0x11bbfc); break;
                case 4: if (e->rd8(B + 0x119721)) { e->f47b20(); pp = G(0x11bbfc); } break;
                default: bail = true;
                }
            }
            if (!bail) {
                if (!pp || (e->rd32(pp) == 0 && e->f55440() == 0)) S(0xfada4, 1);
                bool up = G(0xfada4) != 0;
                if (prev == 0 ? up : !up) {
                    u32 h = e->ord10048();
                    if (prev == 0) { e->f3ccc0(h, B + 0xD0680); S(0x11c2f8, 1); }
                    else { e->f3cc60(h, B + 0xD0680); S(0x11c2f8, 0); }
                    S(0x11c2f4, 0);
                }
            }
        }
    }
    static const u32 T[5][2][2] = {{{0xE2ac0, 0xE2ad8}}, {{0xE3ac8, 0xE3ae0}}, {{0xE5570, 0xE5588}},
        {{0xE8008, 0xE8020}, {0xEa550, 0xEa568}}, {{0xEcfe8, 0xEd000}, {0xEefe0, 0xEeff8}}};
    if (mode >= 0 && mode <= 4) {
        int k = (mode >= 3 && e->ord10227()) ? 1 : 0;
        S(0x11c05c, B + T[mode][k][0]); S(0x11c060, B + T[mode][k][1]);
    }
    S(0x11c058, (u32)((i32)e->rd32(G(0x11c05c)) - 1));
    if (mode == 5) S(0x11c058, 1);
}
}
