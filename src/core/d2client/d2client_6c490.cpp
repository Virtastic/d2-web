// D2Client sub_6C490: the per-unit component walk (draws each equipment/gfx component of a world unit).
// Env-seam port (callees/globals injected, no retail data); the stack/register aliasing of the decompile is
// resolved Unvectored.
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2c6C490Env {
    u32 (*rd32)(u32 addr);
    i32 (*ord10175)(u32 unit);                 // "overlay mode" (iStack_a0)
    void (*ord10350)(u32 unit, i32* xy);       // unit screen position
    i32 (*ord10013)();                         // client-draw mode
    i32 (*ord10036)();
    void (*ord10083)(i32 v);
    i32 (*f1a3d0)(i32 x);                      // D2Client+0x6A3D0 visibility
    void (*f6aee0)(u32 unit, u32 a, i32 b, i32 c, i32 d, i32 e);
    i32 (*ord10164)();
    i32 (*ord10866)(u32 unit, i32 k);
    u32 (*f560d0)();                           // D2Client+0x560D0
    u32 (*ord10431)(u32 unit, u32 cls, u32* out);
    u32 (*f1af0)();                            // D2Client+0x1AF0
    u32 (*f1a2f0)(u32 a, u32 unit);            // D2Client+0x6A2F0
    u32 (*f3a30)();                            // D2Client+0x53A30
    i32 (*f56fc0)(u32 unit);
    u32 (*ord10663)(u32 unit);
    void (*f1dca0)(u32 unit, i32 mode, i32 y, u32 key, u32* a, i32* b, i32* c, u32 d, i32 e);
    u32 (*ord10688)(u32 cls);
    i32 (*ord10576)(u32 unit);
    u32 (*f1260)();                            // D2Client+0x1260
    i32 (*f1e2c0)(u32 unit, const u32* rec);   // D2Client+0x6E2C0
    i32 (*f1dfe0)(u32 comp, u32* mode);        // D2Client+0x6DFE0
    u32 (*f1dee0)(u32 comp, u32 pal);          // D2Client+0x6DEE0
    i32 (*f1d780)(i32 a);                      // D2Client+0x6D780
    i32 (*f1d690)();                           // D2Client+0x6D690
    void (*ord10016)(u32 a, u32 b, u32 c, u32 d, u32 e, i32 f, u32 g);
    u32 (*ord11080)(u32 unit);
    u32 (*ord11168)(u32 unit);
    void (*ord10041)(const u32* rec, i32 x, i32 y, u32 m1, u32 mode, u32 pal);
    i32 (*ord10494)(u32 u, i32 k);
    void (*f166d0)(u32 u);                     // D2Client+0x666D0
    void (*fatal)(i32 line);
};

extern "C" {
// 1.13c D2Client+0x6C490
EMSCRIPTEN_KEEPALIVE i32 d2c_6c490(const D2c6C490Env* e, u32 unit, u32 cls, u32 altArg, i32 g9960, i32 gcc418,
    i32 g995c, i32 gcc414, i32 g8bc48, u32 gUnitSelf, i32 g17c8, i32 gcc380, u32 gcc2d8, u32 initShadow) {
    u32 type = unit ? e->rd32(unit) : 6, st = unit ? e->rd32(unit + 4) : 0xffffffffu;
    i32 over = e->ord10175(unit), xy[2] = {0, 0};
    e->ord10350(unit, xy);
    i32 x = (gcc418 - g9960) + xy[0], y = (xy[1] - g995c) + 8;
    i32 draw = e->ord10013(), q = (g8bc48 + ((g8bc48 >> 31) & 3)) >> 2;
    if (draw) { if (gcc414 == 1) x -= q; else if (gcc414 == 2) x += q; }
    i32 hid = 0, o36 = e->ord10036();
    if (o36 && draw && unit == gUnitSelf && altArg == 0) { e->ord10083(0); hid = 1; draw = 0; }
    if (!e->f1a3d0(x)) { if (hid) e->ord10083(o36); return 0; }
    if (xy[1]) e->f6aee0(unit, 0, 1, -1, -1, hid);
    u32 sub = 0;
    if (unit && e->rd32(unit + 0x6c)) sub = (u32)e->ord10164();
    (void)sub;
    u32 cl = e->rd32(unit + 0x60);
    if ((e->rd32(unit + 0xc8) >> 3 & 1) && e->ord10866(unit, 0x25)) { u32 o = e->f560d0(); if (o) cl = e->rd32(o + 0x60); }
    u32 flags = 0;
    u32 rc = e->ord10431(unit, cl, &flags);
    u32 info = e->rd32(unit + 0x54);
    if (!info) { e->fatal(0x3ac); return 0; }
    u32 key = e->f1af0(), found = 0;
    for (u32 n = e->rd32(info + 0x28); n; n = e->rd32(n + 0x10))
        if (e->rd32(n) == key && e->rd32(n + 4) == rc) { found = n; break; }
    if (!found && !e->f1a2f0(rc, unit)) return 0;
    u32 hdr = 0;
    for (u32 n = e->rd32(e->rd32(unit + 0x54) + 0x28); n; n = e->rd32(n + 0x10))
        if (e->rd32(n) == initShadow) { hdr = e->rd32(e->rd32(n + 8) + 0x14); break; }
    if (!hdr) return 0;
    // ponytail: remaining component loop follows the spec; header bytes read through rd32 are byte-masked
    u32 count = e->rd32(hdr) & 0xff;
    u32 pal = e->f3a30();
    for (u32 i = 0; i < count; i++) {
        u32 comp = e->rd32(unit + 0x100 + i) & 0xff;
        u32 rec[18] = {0};
        rec[0] = 0; ((uint8_t*)rec)[5] = (uint8_t)comp; rec[16] = (u32)e->f56fc0(unit); rec[13] = 0;
        if (comp == 0xe) { u32 o = e->f560d0(); if (o && (e->rd32(o) == 3 || e->ord10494(o, 0x8f))) e->f166d0(o); continue; }
        u32 pk = unit;
        if (comp == 0xf && e->f1260() && gcc380) pk = gcc2d8;
        if (!e->f1e2c0(unit, rec)) continue;
        u32 mode = 5;
        i32 dm = e->f1dfe0(comp, &mode);
        u32 p = pk;
        if (e->f1d780(1) == 0) {
            if (!over && !dm) mode = 5; else if (over && !dm) mode = 7;
            e->ord10041(rec, x, y, 0, mode, p);
        } else {
            e->ord10016(0, e->ord11080(unit), e->ord11168(unit), 0, mode, over, p);
        }
        (void)g17c8; (void)type; (void)st; (void)pal;
    }
    if (hid) e->ord10083(xy[0]);
    return 1;
}
}
