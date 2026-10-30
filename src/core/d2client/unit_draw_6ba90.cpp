// D2Client+0x6BA90: per-unit draw (unit in EAX). Env-seam port in the b56a style: callees injected.
// Unnamed ordinals keep their number; opaque helpers are named by RVA.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2cUnitDrawEnv {
    u32 (*rd32)(u32 addr);
    uint8_t (*rd8)(u32 addr);
    i32 (*ord10494)(u32 unit, i32 code); i32 (*ord10036)(); i32 (*ord10013)(); i32 (*ord10012)(i32 x, i32 y);
    void (*ord10083)(u32 handle);                      // takes the Ordinal_10036 result (or the f1a80 result 0), never the unit
    u32 (*f1a80)();
    void (*f6aa30)(u32 unit);
    i32 (*ord10651)(u32 unit); i32 (*ord11142)(u32 unit);
    i32 (*ord11080)(u32 unit); i32 (*ord11168)(u32 unit);
    void (*ord10023)(i32 x, i32 y, i32 z, i32* ox, i32* oy);  // Ordinal_10023 (0xD084); result unused
    u32 (*ord10688)(u32 id);
    i32 (*f6a3d0)(u32 unit, i32 x, i32 y, i32 one);   // 0x6A3D0 (EAX = unit)
    i32 (*ord10175)(u32 unit);                         // D2Common: the unit's mode
    void (*ord10350)(u32 unit, u32* type, u32* id, i32* mode, u32 line);  // D2Common (file, line 0x926)
    void (*ord10569)(u32 unit, u32* a, u32* b);        // D2Common: the two outputs feed the cel lookups
    i32 (*ord10866)(u32 unit, i32 state);              // D2Common: state 0x25 test
    u32 (*f560d0)(u32 unit);                           // 0x560D0 (EAX = unit): the unit whose +0x60 is used
    u32 (*ord10431)(u32 unit, u32 p60, u32* out, i32 mode, i32 z);
    u32 (*f19910)(u32 unit, i32 m1);                   // 0x69910 (EAX = unit, ECX = -1)
    u32 (*f1a2f0)(u32 unit, u32 tok, i32 m1);          // 0x6A2F0 (EDX = unit, ECX = tok, EAX = -1)
    u32 (*f19090)(u32 unit, i32 mode);                 // 0x69090 (EAX = unit, ECX = mode)
    u32 (*ord10663)(u32 unit);
    void (*f1dca0)(u32 unit, u32 type, u32 id, i32 mode, i32* cls, i32* o2, i32* o3, u32 b2, u32 one);
    u32 (*f1d6d0)(u32 map, i32 a, u32 b);              // 0x6D6D0 (EAX = map)
    i32 (*f1ea50)(u32 unit, u32 map, u32 buf, i32* cnt, i32 cls, u32 b);  // 0x6EA50 (EBX = unit)
    i32 (*f1d740)(u32 map, u32 comp);                  // 0x6D740 (ECX = map, EDI = component)
    i32 (*f1e2c0)(u32 unit, u32* rec, u32* out, i32 m1);  // 0x6E2C0 (EAX = -1)
    void (*ord10011)(const u32* rec, i32 x, i32 y);    // D2gfx: draw the component
    void (*fatal)(i32 line);                           // Ordinal_10265/10024 + _exit(-1)
};

extern "C" {
// 1.13c D2Client+0x6BA90
EMSCRIPTEN_KEEPALIVE i32 d2c_6ba90(const D2cUnitDrawEnv* e, u32 unit, u32 gBBFC, i32 g9960, i32 gcc418,
                                   i32 g995c, i32 gcc414, i32 g8bc48) {
    auto R = [&](u32 a, u32 off) -> i32 { return a ? (i32)e->rd32(a + off) : 0; };
    if (unit && ((e->rd32(unit + 0xc4) >> 5) & 1)) return 0;
    if (e->ord10494(unit, 0x92)) return 0;
    i32 type = unit ? (i32)e->rd32(unit) : -1;
    if (!unit || type < 0 || type > 2) { e->f6aa30(unit); return 1; }
    i32 v36 = e->ord10036();
    i32 fA = 0;                                        // "pushed" flag: set only by this block, undone by ord10083(v36)
    if (v36 && e->ord10013() && unit == gBBFC) {
        u32 r = e->f1a80();
        if (!r) { e->ord10083(r); fA = 1; }
    }
    i32 c13 = e->ord10013();
    u32 info = e->rd32(unit + 0x54);
    u32 ext = info ? e->rd32(info + 0x30) : 0;
    i32 x, y;
    if (!c13) {
        i32 half = (ext ? R(ext, 0x3c) : 0) / 2;
        x = e->ord10651(unit) + half;
        y = e->ord11142(unit) + half;
    } else {
        i32 half = (ext ? R(ext, 0xc) : 0) / 2;
        i32 X = e->ord11080(unit) - half, Y = e->ord11168(unit) - half;
        if (!e->ord10012(X, Y)) { if (fA) e->ord10083((u32)v36); return 0; }
        e->ord10023(X, Y, 0, &x, &y);
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
    if (c13) {
        i32 q = (g8bc48 + ((g8bc48 >> 31) & 3)) >> 2;
        if (gcc414 == 1) dx = sx - q; else if (gcc414 == 2) dx = sx + q;
    }
    auto bail = [&]() -> i32 { if (fA) e->ord10083((u32)v36); return 0; };
    if (!e->f6a3d0(unit, dx, sy, 1)) return bail();
    u32 ty = (u32)type, id = e->rd32(unit + 4);
    i32 mode = e->ord10175(unit);
    e->ord10350(unit, &ty, &id, &mode, 0x926);
    u32 l40 = 0, l44 = 0;
    e->ord10569(unit, &l40, &l44);
    i32 st = (i32)((e->rd32(unit + 0xc8) >> 3) & 1);
    u32 p60 = e->rd32(unit + 0x60);
    if (st && e->ord10866(unit, 0x25)) { u32 o = e->f560d0(unit); if (o) p60 = e->rd32(o + 0x60); }
    u32 l4c = 0;
    u32 tok = e->ord10431(unit, p60, &l4c, mode, 0);
    if (!e->f19910(unit, -1) && !e->f1a2f0(unit, tok, -1)) return 0;
    u32 map = e->f19090(unit, mode);
    if (!map) return 0;
    i32 cls = (i32)(e->ord10663(unit) & 0xff);
    i32 o2 = c13, o3 = st;                             // retail reuses these two stack slots as out-params
    e->f1dca0(unit, ty, id, mode, &cls, &o2, &o3, e->rd8(map + 2), 1);
    u32 buf = e->f1d6d0(map, o2, l40);
    if (!buf) { e->fatal(0x94e); return 0; }
    i32 cnt = 0;
    if (e->f1ea50(unit, map, buf, &cnt, cls, l40) <= 0) return bail();
    for (u32 i = 0; i < e->rd8(map); i++) {
        uint8_t b = e->rd8(buf + i);
        if (b >= 0x10) { e->fatal(0x960); return 0; }
        if (e->f1d740(map, b)) {
            u32 rec[18] = {0};
            rec[0] = l40;
            ((uint8_t*)rec)[5] = b;
            rec[16] = (u32)cls;
            u32 l48 = 0;
            if (e->f1e2c0(unit, rec, &l48, -1)) e->ord10011(rec, dx, sy);
        }
    }
    if (fA) e->ord10083((u32)v36);
    return 1;
}
}
