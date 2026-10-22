// D2Client+0x6BA90: per-unit draw (unit in EAX). Env-seam port in the b56a style: callees injected.
// Unnamed ordinals keep their number; opaque helpers are named by RVA.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cUnitDrawEnv {
    u32 (*rd32)(u32 addr);
    uint8_t (*rd8)(u32 addr);
    i32 (*ord10494)(); i32 (*ord10036)(); i32 (*ord10013)(); i32 (*ord10012)();
    void (*ord10083)(u32 unit);
    u32 (*f1a80)();
    void (*f6aa30)(u32 unit);
    i32 (*ord10651)(u32 unit); i32 (*ord11142)(u32 unit);
    i32 (*ord11080)(u32 unit); i32 (*ord11168)(u32 unit);
    i32 (*ord10023)(i32 x, i32 y, i32 z);              // returns non-zero iff out-flag A set (flag B in *hi)
    i32 (*ord10023b)();                                // second out flag of the last ord10023
    u32 (*ord10688)(u32 id);
    i32 (*f6a3d0)(i32 x);
    void (*prelude)(u32 unit);                         // Ordinal_10175 (result in *ord10175), 10350, 10569
    i32 (*ord10175)();
    i32 (*ord10866)(); void (*f560d0)();
    u32 (*ord10431)(); u32 (*f19910)(i32 a); u32 (*f1a2f0)(u32 a, u32 unit); u32 (*f19090)(u32 unit);
    u32 (*ord10663)();
    void (*f1dca0)(u32 unit, u32 type, u32 id, u32 b2, i32* o1, i32* o2, i32* o3, u32 b);
    u32 (*f1d6d0)(i32 a, i32 b);
    i32 (*f1ea50)(u32 map, u32 buf, i32* cnt, u32 cls, i32 a);
    i32 (*f1d740)(u32 map);
    i32 (*f1e2c0)(u32 unit, const u32* rec);
    void (*ord10011)(const u32* rec, i32 x, i32 y);
    void (*fatal)(i32 line);                           // Ordinal_10265/10024 + _exit(-1)
};

extern "C" {
// 1.13c D2Client+0x6BA90
EMSCRIPTEN_KEEPALIVE i32 d2c_6ba90(const D2cUnitDrawEnv* e, u32 unit, u32 gBBFC, i32 g9960, i32 gcc418,
                                   i32 g995c, i32 gcc414, i32 g8bc48) {
    auto R = [&](u32 a, u32 off) -> i32 { return a ? (i32)e->rd32(a + off) : 0; };
    if (unit && ((e->rd32(unit + 0xc4) >> 5) & 1)) return 0;
    if (e->ord10494()) return 0;
    i32 type = unit ? (i32)e->rd32(unit) : -1;
    if (!unit || type < 0 || type > 2) { e->f6aa30(unit); return 1; }
    i32 v36 = e->ord10036();
    if (v36 && e->ord10013() && unit == gBBFC && !e->f1a80()) e->ord10083(unit);
    u32 info = e->rd32(unit + 0x54);
    u32 ext = info ? e->rd32(info + 0x30) : 0;
    i32 x, y, fA = 0, fB = 0;
    if (!e->ord10013()) {
        i32 half = (ext ? R(ext, 0x3c) : 0) / 2;
        x = e->ord10651(unit) + half;
        y = e->ord11142(unit) + half;
    } else {
        i32 half = (ext ? R(ext, 0xc) : 0) / 2;
        x = e->ord11080(unit) - half;
        y = e->ord11168(unit) - v36;
        if (!e->ord10012()) { e->ord10083(unit); return 0; }
        fA = e->ord10023(x, y, 0); fB = e->ord10023b();
        x += g9960 - gcc418;
        y += 8 + g995c;
    }
    if (fA) y -= 8;
    x += ext ? R(ext, 0x34) : 0;
    y += ext ? R(ext, 0x38) : 0;
    i32 sx = (gcc418 - g9960) - 2 + x;
    i32 sy = (y - g995c) + 8;
    if (type == 2) { u32 t = e->ord10688(e->rd32(unit + 4)); sx += R(t, 0x148); sy += R(t, 0x14c); }
    i32 dx = sx;
    if (fB) {
        i32 q = (g8bc48 + ((g8bc48 >> 31) & 3)) >> 2;
        if (gcc414 == 1) dx = sx - q; else if (gcc414 == 2) dx = sx + q;
    }
    auto bail = [&]() -> i32 { if (fA) e->ord10083(unit); return 0; };
    if (!e->f6a3d0(dx)) return bail();
    u32 id = e->rd32(unit + 4);
    i32 u175 = e->ord10175(); e->prelude(unit);
    if (((e->rd32(unit + 0xc8) >> 3) & 1) && e->ord10866()) e->f560d0();
    u32 a = e->ord10431();
    if (!e->f19910(-1) && !e->f1a2f0(a, unit)) return 0;
    u32 map = e->f19090(unit);
    if (!map) return 0;
    u32 cls = e->ord10663() & 0xff;
    i32 o1 = 0, o2 = 0, o3 = 0;
    e->f1dca0(unit, (u32)type, id, 0, &o1, &o2, &o3, e->rd8(map + 2));
    u32 buf = e->f1d6d0(o2, o1);
    if (!buf) { e->fatal(0x94e); return 0; }
    i32 cnt = 0;
    if (e->f1ea50(map, buf, &cnt, cls, o1) <= 0) return bail();
    uint8_t n = e->rd8(map);
    for (i32 i = 0; n && i < n; i++) {
        uint8_t b = e->rd8(buf + i);
        if (b > 0xf) { e->fatal(0x960); return 0; }
        if (e->f1d740(map)) {
            u32 rec[18] = {0};
            rec[0] = (u32)o1;
            ((uint8_t*)rec)[5] = b;
            rec[16] = cls;
            if (e->f1e2c0(unit, rec)) e->ord10011(rec, dx, sy);
        }
    }
    (void)u175;
    if (fA) e->ord10083(unit);
    return 1;
}
}
