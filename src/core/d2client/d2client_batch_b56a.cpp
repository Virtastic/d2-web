// D2Client batch b56-cli-a: env-seam ports (callees/globals injected, no retail data).
// Skipped (register args / x87 / stack-hack signatures): 64990 64FD0 65760 666D0 69410 6A220 6AA30 6B300 6BA90 6C490.
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cB56aEnv {
    u32 (*rd32)(u32 addr);                                  // guest 32-bit read
    i32 (*ord10013)();                                      // is-client-draw mode
    i32 (*ord10175)(u32 unit);
    i32 (*ord10651)(u32 unit);
    i32 (*ord11142)(u32 unit);
    i32 (*ord11168)(u32 unit, i32 a, i32* x, i32* y);
    i32 (*ord11080)(u32 unit, i32 v);
    void (*ord10023)(i32 v);
    u32 (*ord10663)(u32 unit);
    i32 (*f1e2c0)(u32 unit, const u32* rec);                // D2Client+0x6E2C0
    void (*ord10041)(const u32* rec, i32 x, i32 y, i32 m1, i32 mode, i32 z);
    void (*fatal)(i32 line);                                // Ordinal_10265/10024 + _exit(-1)
};

extern "C" {
// 1.13c D2Client+0x6AEE0
EMSCRIPTEN_KEEPALIVE void d2c_6aee0(const D2cB56aEnv* e, u32 unit, i32 p2, i32 p3, i32 p4, i32 p5, i32 p6,
    const uint8_t* tbl, i32 tblCount, i32 g9960, i32 gcc418, i32 g995c, i32 gcc414, i32 g8bc48) {
    auto R = [&](u32 a, u32 off) { return a ? (i32)e->rd32(a + off) : 0; };
    u32 info = unit ? e->rd32(unit + 0x54) : 0;
    if (!info) { e->fatal(0x271); return; }
    i32 draw = e->ord10013();
    u32 node = e->rd32(info + 0x2c);
    if (!node) return;
    i32 utype = (i32)e->rd32(unit);
    i32 x, y, xb = 0;
    if (!draw || (utype == 4 && e->ord10175(unit) != 3)) {
        x = e->ord10651(unit) - (g9960 - gcc418);
        y = e->ord11142(unit) - (g995c - 8);
        if (utype == 4 && p4 != -1 && p5 != -1) { xb = p4; y = p5; } else xb = x;
    } else {
        i32 ax = 0, ay = 0;
        i32 v = e->ord11168(unit, 0, &ax, &ay);
        v = e->ord11080(unit, v);
        e->ord10023(v);
        i32 q = (g8bc48 + ((g8bc48 >> 31) & 3)) >> 2;
        if (gcc414 == 1) ax += -q; else if (gcc414 == 2) ax += q;
        xb = ax; y = ay + 0x10;
    }
    u32 ext = info ? e->rd32(info + 0x30) : 0;
    xb += ext ? R(ext, 0x34) : 0;
    y += (ext ? R(ext, 0x38) : 0) + (ext ? R(ext, 0x3c) : 0);
    if (p6) y -= 8;
    for (;;) {
        bool ok = true;
        i32 f34 = R(node, 0x34);
        if (f34 == 0 || (ok = p3 != 0, f34 == 0)) { if (p3 != 0) ok = false; }
        i32 nt = (i32)e->rd32(node);
        if (nt == 6 && R(node, 8) == 0) ok = false;
        if ((nt != 8 || R(node, 0x3c) != 0) && ok) {
            i32 f18 = R(node, 0x18), idx = R(node, 4);
            if (idx < 0 || idx >= tblCount || !tbl) { e->fatal(0x2d1); return; }
            const uint8_t* ent = tbl + idx * 0x84;
            u32 rec[18] = {0};
            rec[5] = (u32)idx;
            rec[16] = e->ord10663(unit) & 0xff;
            rec[13] = 0;
            ((uint8_t*)rec)[5] = 1;
            rec[0] = (u32)(f18 >> 8);
            if (!e->f1e2c0(0, rec)) return;
            if ((f18 >> 8) < (R(node, 0x1c) >> 8))
                e->ord10041(rec, R(node, 0x20) + xb, R(node, 0x24) + y, p2, ent[0x7c], 0);
        }
        node = e->rd32(node + 0xa4);
        if (!node) return;
    }
}
}
