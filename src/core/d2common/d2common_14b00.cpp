// 1.13c D2Common+0x14B00 -- placement step (ecx = the work record): start direction and direction of the current rectangle become 3, the record seed steps once, and the rectangle goes to the right edge of its anchor: an odd seed puts it 8 above the bottom-aligned line and stores 0x800000 at image offset 0xA146C, an even seed puts it 8 above the anchor top and stores 0x400000. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_a146c;
namespace d2common {
static uint32_t roll(GE& e, uint32_t t) { const uint64_t v = (uint64_t)e.rd32(t) * 0x6ac690c5u + e.rd32(t + 4); e.wr32(t, (uint32_t)v); e.wr32(t + 4, (uint32_t)(v >> 32)); return (uint32_t)v; }
uint32_t f14b00(GE& e, uint32_t t)
{
    const uint32_t i = e.rd32(t + 0x1ec), o = t + i * 16, d = t + 0xfc + i * 4, s = t + 0x138 + i * 4;
    e.wr32(s, 3);
    e.wr32(d, e.rd32(s));
    const uint32_t p = t + 8 + e.rd32(e.rd32(t + 0xf8) + 8 + i * 16) * 16;
    const uint32_t px = e.rd32(p), py = e.rd32(p + 4), pw = e.rd32(p + 8), ph = e.rd32(p + 12), sw = e.rd32(o + 0x10), sh = e.rd32(o + 0x14);
    (void)px; (void)py; (void)pw; (void)ph; (void)sw; (void)sh;
    auto set = [&](uint32_t x, uint32_t y) { e.wr32(o + 8, x); e.wr32(o + 0xc, y); };
    const uint32_t v = roll(e, t);
    if (v & 1) {
    switch (e.rd32(d)) {
    case 0: set(px, ph + py); break;
    case 1: set(px - sw, py); break;
    case 2: set(pw - sw + px, py - sh); break;
    case 3: set(pw + px, ph - sh + 8 + py); break;
    default: break;
    }
        g_d2common_a146c = 0x800000;
    } else {
    switch (e.rd32(d)) {
    case 0: set(pw - sw + px, ph + py); break;
    case 1: set(px - sw, ph - sh + py); break;
    case 2: set(px, py - sh); break;
    case 3: set(pw + px, py - 8); break;
    default: break;
    }
        g_d2common_a146c = 0x400000;
    }
    return 1;
}
}
