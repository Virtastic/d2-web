// 1.13c D2Game+0x55320 (player-save header reader, versions 0x47..0x60): ecx = unused, edx = the 0x82-byte header, stack = (self, cl, &unit, o6, o7, o8, o9, o10, o11, o12), ret 0x28.
// cl is the client record; EAX = cl for the callees 0x49A20, 0x49D70, 0x53720, 0x49A80, 0x494E0. The header is copied to a local first (rep movs). Callees go through the D2Seam.
// 1.13c deltas against the decompile's reading: the hot-key loop passes 0xFFFFFFFF (not 0xFFFF) for an 0xFF key (`or eax,-1`); 0x49D70 takes its argument in CL with
// the rest of ECX holding (flags >> 13) &~0xFF; 0xB23A0 takes EAX = self; 0x7B0A0 is passed (0x49A80 result, unit, cl).
// flag = DAT_6fd31c3c (0xF1C3C... module 0x111C3C), ba0 = *(*(0x6fd1829c)) + 0xBA0 (the charstats-like row count).
#include <cstdlib>
#include "d2game_seam4e.h"
static uint32_t d2k_hk(uint32_t v) { return (v < 0x9c || v > 0xd8) ? (v == 0xff ? 0xffffffffu : v) : v + 0x41; }
extern "C" uint32_t d2g_55320(uint32_t hdrp, uint32_t self, uint32_t cl, uint32_t pUnit, uint32_t o6, uint32_t o7, uint32_t o8,
                              uint32_t o9, uint32_t o10, uint32_t o11, uint32_t o12, uint32_t flag, uint32_t ba0, const D2Seam* e)
{
    uint8_t h[0x82];
    memcpy(h, (const void*)(uintptr_t)hdrp, 0x82);
    auto u16 = [&](int o) { uint16_t v; memcpy(&v, h + o, 2); return (uint32_t)v; };
    auto u32 = [&](int o) { uint32_t v; memcpy(&v, h + o, 4); return v; };
    d2wr32(o12, 0);
    d2wr16(cl + 10, 0);
    if (u32(0) != 0xaa55aa55u) { if (!flag) return 1; d2wr32(o6, 1); return 0; }
    if (u16(0x20) != 0x82) { if (flag) { d2wr32(o6, 1); return 0; } return 0xe; }
    const uint32_t ver = u32(4);
    d2wr32(o7, ver);
    if (ver < 0x47 || ver > 0x60) { if (!flag) return 1; d2wr32(o6, 1); return 9; }
    h[0x17] = 0;
    const uint32_t nm = e->call(0x49A20, cl);
    d2wr8(nm + 0xf, 0);
    if (e->call(0x2813, 0, 0, 0, 0, 0, nm, (uint32_t)(uintptr_t)(h + 8)) != 0) { d2wr32(o6, 0); return 1; }
    const uint32_t fl = u32(0x18), bit5 = fl & 0x20;
    if (!bit5) { if (d2rd32(self + 0x70)) return 0x17; }
    else if (!d2rd32(self + 0x70)) return 0x18;
    if (!(fl & 4)) { if (e->call(0xB23A0, self) & 0x800) return 0x13; }
    else { if (fl & 8) return 0x15; if (!(e->call(0xB23A0, self) & 0x800)) return 0x14; }
    const uint32_t lv = fl >> 8 & 0x1f, mode = d2rd8(self + 0x6d);
    if (lv < 8 && mode > 1) return 0x12;
    if ((bit5 && lv < 5 && mode) || (lv < 4 && mode)) return 0x11;
    char name[28]; memcpy(name, h + 0x5f, 27); name[27] = 0;
    d2wr16(cl + 0x45e, u16(0x59));
    d2wr32(cl + 0x460, u32(0x5b) & 0xffffff);
    for (uint32_t i = 0;; i++) { d2wr8(cl + 0x464 + i, (uint8_t)name[i]); if (!name[i]) break; }
    d2wr8(cl + 0x481, h[0x7c]); d2wr8(cl + 0x480, h[0x7b]); d2wr8(cl + 0x482, h[0x7d]);
    if (fl & 1) { d2wr32(o6, 1); d2wr16(cl + 10, fl & 0xfffe); return 0; }
    d2wr8(cl + 0x45c, fl >> 0xd & 1);
    d2wr32(o6, 0);
    d2wr16(cl + 10, fl);
    const uint32_t b48 = h[0x58], hi = b48 >> 4;
    d2wr8(cl + 8, u16(0x22));
    if ((b48 & 0xf) > 4 || hi > 2) return 9;
    if (d2rd8(self + 0x6a) == 3 && d2rd32(self + 0x84) == 0 && mode == hi) d2wr32(self + 0x7c, u32(0x7e));
    if (mode == hi) e->call(0x49D70, cl, ((fl >> 13) & ~0xFFu) | (b48 & 0xf));
    else d2wr8(cl + 0x1ac, 0);
    const uint32_t unit = e->call(0xE1D90, 0, 0, u16(0x22), 0, 0, 0, 0, self, 0, 0, 0, 0);
    d2wr32(pUnit, unit);
    e->call(0x53720, cl, unit, 0, 0, 0, self);
    e->call(0x7B0A0, 0, 0, 0, 0, 0, e->call(0x49A80, cl), d2rd32(pUnit), cl);
    d2wr32(o8, 0x10);
    d2wr32(o9, ba0);
    if (u16(0x1e) > 0x10 || (int32_t)u16(0x1c) > (int32_t)ba0) {
        if (flag) { d2wr32(o6, 1); return 0; }
        return 1;
    }
    if (u16(0x1e) != 0x10) d2wr32(o8, u16(0x1e));
    if (u16(0x1c) != ba0) d2wr32(o9, u16(0x1c));
    for (uint32_t i = 0; i < 8; i++)
        e->call(0x494E0, cl, i, 0, 0, 0, d2k_hk(h[0x46 + i * 2]), h[0x47 + i * 2], 0xffffffffu);
    d2wr32(o10, d2k_hk(h[0x56]));
    d2wr32(o11, d2k_hk(h[0x57]));
    d2wr32(o12, u16(0x20));
    return 0;
}
