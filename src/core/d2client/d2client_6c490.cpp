// D2Client sub_6C490: the per-unit component walk (draws each equipment/gfx component of a world unit).
// Env-seam port (callees/globals injected, no retail data), read off the D2Client.dll disassembly (the decompile aliases its
// stack slots). Convention: EAX = y base, stack (unit, a1, x base, a3, a4), ret 0x14. Frame slots are named by their
// [esp+N] offset after the prologue (sub esp,0xa8 and four pushes).
#include <cstdint>
#include <emscripten.h>
typedef int32_t i32; typedef uint32_t u32;

struct D2c6C490Env {
    u32 (*rd32)(u32 addr);
    u32 (*rd8)(u32 addr);
    i32 (*ord10175)(u32 unit);                                  // IAT thunk 0xC65E
    void (*ord10350)(u32 unit, u32* type, u32* cls, u32* mode); // 0xC658 (unit, &[10], &[44], &[1c], file, 0x52B)
    i32 (*ord10013)();                                          // 0xD07E
    i32 (*ord10036)();                                          // 0xD11A
    void (*ord10083)(i32 v);                                    // 0xD0CC
    i32 (*f6a3d0)(u32 unit, i32 x, i32 y, i32 flag);            // eax = unit, stack x, y, flag
    void (*f6aee0)(u32 unit, u32 a1, i32 open, i32 c, i32 d, i32 hid);
    void (*ord10569)(u32 unit, u32* a, u32* b);                 // 0xC5EC (unit, &[4c], &[68])
    u32 (*ord10164)(u32 ecx);                                   // 0xD3FC, ecx = unit+0x6C - 1
    i32 (*ord10866)(u32 unit, i32 k);                           // 0xCBDA
    u32 (*f560d0)(u32 unit);                                    // eax = unit
    u32 (*ord10431)(u32 unit, u32 cls, u32* out, u32 mode, u32 zero);   // 0xC2AA
    u32 (*f1af0)(u32 unit);                                     // esi = unit
    u32 (*f6a2f0)(u32 eax, u32 rc, u32 unit);                   // eax = -1, ecx = rc, edx = unit
    u32 (*f53a30)(u32 unit);                                    // eax = unit
    u32 (*fa6fc0)(u32 unit);
    u32 (*ord10663)(u32 unit);                                  // 0xC610 (low byte used)
    void (*f6dca0)(u32 unit, u32 type, u32 cls, u32 mode, u32* p5c, u32* p28, u32* p50, u32 h2, i32 one);
    u32 (*ord10688)(u32 cls);                                   // 0xC226, an object's record
    i32 (*ord10576)(u32 unit);                                  // 0xCC16
    u32 (*f1260)(u32 cls);                                      // eax = unit+4
    i32 (*f6e2c0)(u32 unit, u32* rec, u32* p64);
    i32 (*f6dfe0)(u32 unit, u32 ebx, u32 comp, u32* mode);      // eax = unit, ebx = [64]
    u32 (*f6dee0)(u32 unit, u32 comp, u32 pal);                 // eax = unit
    i32 (*f6d780)(u32 hdr);                                     // ecx = hdr
    u32 (*f6d690)(u32 hdr, u32 comp);                           // eax = hdr, esi = comp
    void (*ord10016)(const u32* rec, u32 a, u32 b, u32 a1, u32 mode, i32 g414, u32 pal);   // 0xD120
    u32 (*ord11080)(u32 unit);                                  // 0xC1AE
    u32 (*ord11168)(u32 unit);                                  // 0xC232
    void (*ord10041)(const u32* rec, i32 x, i32 y, u32 a1, u32 mode, u32 pal);             // 0xD096
    i32 (*ord10494)(u32 u, i32 k);                              // 0xC1FC
    void (*f666d0)(u32 eax, u32 ecx, u32 a1);                   // eax = 1, ecx = owner
    void (*fatal)(i32 line);                                    // 0xBF92 / 0xBF80 / 0x3664
};

extern "C" {
// 1.13c D2Client+0x6C490
EMSCRIPTEN_KEEPALIVE i32 d2c_6c490(const D2c6C490Env* e, i32 yBase, u32 unit, u32 a1, i32 xBase, u32 a3, u32 a4,
    i32 g9960, i32 gcc418, i32 g995c, i32 gcc414, i32 g8bc48, u32 gHero, i32 g17c8, i32 gcc380, u32 gcc2d8) {
    u32 type = 6, cls = 0xffffffffu;                            // [10], [44]
    if (unit) { type = e->rd32(unit); cls = e->rd32(unit + 4); }
    u32 mode = (u32)e->ord10175(unit);                          // [1c]
    e->ord10350(unit, &type, &cls, &mode);
    i32 x = (gcc418 - g9960) + xBase, y = (yBase - g995c) + 8;  // [18], [24]
    i32 draw = e->ord10013();                                   // [40]
    if (draw) {
        i32 q = (g8bc48 + ((g8bc48 >> 31) & 3)) >> 2;
        if (gcc414 == 1) x -= q; else if (gcc414 == 2) x += q;
    }
    i32 hid = 0;                                                // [38]
    i32 o36 = e->ord10036();                                    // [48]
    if (o36 && draw && unit == gHero && type == 0) { e->ord10083(0); hid = 1; draw = 0; }
    if (!e->f6a3d0(unit, x, y, 0)) { if (hid) e->ord10083(o36); return 0; }
    if (a4) e->f6aee0(unit, a1, 1, -1, -1, hid);
    u32 s4c = 0, s68 = 0;                                       // 10569's out-words
    e->ord10569(unit, &s4c, &s68);
    u32 pal = 0;                                                // [20]
    u32 c6c = e->rd32(unit + 0x6c);
    if (c6c) pal = e->ord10164(c6c - 1);
    u32 rcls = e->rd32(unit + 0x60);
    u32 f58 = e->rd32(unit + 0xc8) >> 3 & 1;
    if (f58 && e->ord10866(unit, 0x25)) { u32 o = e->f560d0(unit); if (o) rcls = e->rd32(o + 0x60); }
    u32 s6c = 0;
    u32 rc = e->ord10431(unit, rcls, &s6c, mode, 0);
    u32 info = e->rd32(unit + 0x54);
    if (!info) { e->fatal(0x3ac); return 0; }
    u32 key = e->f1af0(unit), n = e->rd32(info + 0x28);
    for (; n; n = e->rd32(n + 0x10)) if (e->rd32(n) == key && e->rd32(n + 4) == rc) break;
    if (!n && !e->f6a2f0(0xffffffffu, rc, unit)) return 0;
    info = e->rd32(unit + 0x54);
    if (!info) return 0;
    n = e->rd32(info + 0x28);
    if (!n) return 0;
    while (e->rd32(n) != mode) { n = e->rd32(n + 0x10); if (!n) return 0; }
    u32 hdr = e->rd32(e->rd32(n + 8) + 0x14);                   // [30]
    if (!hdr) return 0;
    if (!pal) pal = e->f53a30(unit);
    u32 r58 = e->fa6fc0(unit);                                  // [58]
    u32 s5c = e->ord10663(unit) & 0xff;                         // [5c]
    u32 s28 = 0, s50 = 0;
    e->f6dca0(unit, type, cls, mode, &s5c, &s28, &s50, e->rd8(hdr + 2), 1);
    u32 h0 = e->rd8(hdr), h1 = e->rd8(hdr + 1);
    u32 comps = hdr + 0x1c + h1 + h0 * (h1 * s28 + s4c + 9);    // [50]
    if (!comps) { e->fatal(0x58e); return 0; }
    u32 obj1 = 0;                                               // [34]
    if (type == 2 && e->rd8(e->ord10688(e->rd32(unit + 4)) + mode + 0x131) == 1) obj1 = 1;
    u32 pal2c = 1;                                              // [2c]
    if (e->ord10576(unit) && (unit != gHero || g17c8 < 4)) pal2c = 0;
    for (u32 i = 0; i < (e->rd8(hdr)); i++) {                   // [28]
        u32 comp = e->rd8(comps + i);                           // [54]
        u32 p = pal;                                            // [14]
        if (comp == 0xe) {
            u32 o = e->f560d0(unit);
            if (o && (e->rd32(o) == 3 || e->ord10494(o, 0x8f))) e->f666d0(1, o, a1);
            continue;
        }
        if (comp == 0xf) {
            p = 0;
            if (type == 1) { u32 t = e->f1260(e->rd32(unit + 4)); if (t && (e->rd8(t + 0x11c)) && gcc380) p = gcc2d8; }
        }
        u32 rec[18] = {0};                                      // [70..b8)
        rec[0] = s4c; rec[16] = s5c; rec[13] = 0; ((uint8_t*)rec)[5] = (uint8_t)comp;
        u32 s64 = 0xffffffffu;
        if (!e->f6e2c0(unit, rec, &s64)) continue;
        u32 m3c = 5;
        i32 dm = e->f6dfe0(unit, s64, comp, &m3c);
        if (pal2c && (type != 0 || e->rd32(unit + 0x6c) != 0x6c)) p = e->f6dee0(unit, comp, p);
        u32 md;
        if (e->f6d780(hdr)) {
            if (a3) md = 1; else if (!dm) md = e->f6d690(hdr, comp); else md = m3c;
            if (draw && obj1) { u32 b = e->ord11080(unit); u32 c = e->ord11168(unit); e->ord10016(rec, b, c, a1, md, gcc414, p); continue; }
        } else if (!r58 && !dm) {
            if (a3) md = 1;
            else if (draw && obj1) { u32 b = e->ord11080(unit); u32 c = e->ord11168(unit); e->ord10016(rec, b, c, a1, 5, gcc414, p); continue; }
            else md = 5;
        } else if (a3) md = 1;
        else if (!dm && r58) {
            md = 7;
            u32 v = (a1 & 0xff) * 2;
            a1 = (a1 & 0xffffff00u) | ((i32)v <= 0x40 ? 0x40u : (i32)v >= 0xff ? 0xffu : v);
        } else md = m3c;
        e->ord10041(rec, x, y, a1, md, p);
    }
    if (a4) e->f6aee0(unit, a1, 0, -1, -1, hid);
    if (hid) e->ord10083(o36);   // 0x6fb1cb0e: the value 10036 returned ([esp+0x48]), as on the early exit
    return 1;
}
}
