// D2Client sub_C1A80: UI-message gate (eax=a, ebx=b register args). Env-seam port; ordinals 10265/10024 unverified (fatal seam).
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cGateEnv {
    u32 base;                                   // D2Client image base (0x6FAB0000 in retail)
    u32 (*rd32)(u32 addr); u32 (*rd8)(u32 addr); void (*wr32)(u32 addr, u32 v); void (*wr8)(u32 addr, u32 v);
    u32 (*tick)();                              // GetTickCount
    void (*fa6210)(); void (*f14a30)(); void (*f14660)(); void (*f47b20)();
    void (*f72790)(u32 i, i32 a, i32 b);        // D2Client+0xC2790
    void (*fatal)(i32 line);                    // Ordinal_10265(line)/10024 + _exit(-1)
};

extern "C" {
// 1.13c D2Client+0xC1A80
EMSCRIPTEN_KEEPALIVE i32 d2c_c1a80(const D2cGateEnv* e, i32 a, i32 b) {
    const u32 B = e->base;
    auto G = [&](u32 r) { return e->rd32(B + r); };
    const i32 n = (i32)G(0xDD5D0);
    if (n != 0x26) { e->fatal(0x139b); return 0; }
    if (!(a == 0 || (a == 2 && e->rd32(B + 0xFAD80 + 4 * b) == 0))) return 1;
    if (b == 5 || b == 9) {
        if (G(0x11BC08) && b == 5 && G(0xFADE0) == 0) return 0;
        if (e->tick() < G(0xFAD50)) return 0;
        if (b == 9) {
            u32 p = G(0x11BBFC);
            if (!p) return 0;
            if (e->rd32(p) == 0 && e->rd32(p + 0x10) == 0x11) {
                e->fa6210(); e->f14a30();
                u32 q = G(0x11B980) + 0x1eb;
                if (!(e->rd8(q) & 4)) return 0;
                e->f14660();
                e->wr8(q, e->rd8(q) | 8);
                e->wr32(B + 0xF8C9C, 0); e->wr32(B + 0x11C39C, 0); e->wr32(B + 0x11C3A0, 0); e->wr32(B + 0x11C3A4, 1);
                return 0;
            }
        }
    }
    if (G(0x113A68) && b != 10 && b != 0x13 && b != 0x11 && b != 6 && b != 7) return 0;
    for (i32 i = 0; i < n; i++) {
        if (e->rd32(B + 0xFAD80 + 4 * i) != 1) continue;
        u32 tbl = G(0xDD538 + 4 * i);
        switch (e->rd32(tbl + 4 * b)) {
        case 0: break;
        case 1: e->f72790((u32)i, 1, 0); break;
        case 2: return 0;
        case 4: if (e->rd8(B + 0x119721)) e->f47b20(); break;
        default: if (b != 0) return 0; e->fatal(0x13ea); return 0;
        }
    }
    return 1;
}
}
