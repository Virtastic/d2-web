// 1.13c D2Client+0x9EC10 (441 B, EAX = owner, stack view, ret 4): lit overlay list draw. owner+0x20 heads a list of nodes
// (+4 x, +8 y, +0xC item, +0x10 next). Per node: an item with state bit 4 whose expiry (+0x2C) is <= now loses bit 4 and gains
// flags bit 8; then an item whose flags (+0x14) have no 0x408 bit and some 0x1C000 bit is drawn. kind = *cel (+0x18); the
// kind's count (table 0xD0A70) of light samples come from 0xA8230 at (node x + dx*8, node y + dy*8), (dx, dy) from the 0x30-byte
// kind rows of 0xDED18 (state bit 0 clear) or 0xDEEF8; each sample packs level | r<<8 | g<<16 | b<<24 into a stack array.
// Position: node x, y when Ordinal 10013 (0xD07E) is nonzero, else view+4 - view+0x24 + item x, item y - view+0x28 + view+8.
// Alpha byte (+0x28) 0xFF draws with 0xD0D2 (cel, x, y, colours, light 0x11C414), else 0xD0D8 (..., alpha); nonzero sets 0x20000.
#include "d2client_light_env.h"
extern "C" void d2c_light_a8230(int32_t, int32_t, uint32_t, uint32_t, uint32_t, uint32_t, const cl::Env&);
extern "C" void d2c_9ec10(uint32_t owner, uint32_t view, uint32_t now, const cl::Env& e) {
    constexpr uint32_t B = cllight::kBase;
    uint32_t n = e.R(owner + 0x20);
    if (!n) return;
    const uint32_t light = e.R(B + 0x11C414);
    const uint32_t mode = e.H(0xd07e);
    uint32_t col[8] = {};
    for (; n; n = e.R(n + 0x10)) {
        const uint32_t it = e.R(n + 0xc);
        uint32_t st = e.R(it + 0x24);
        if ((st & 4) && now >= e.R(it + 0x2c)) { e.W(it + 0x24, st & ~4u); e.W(it + 0x14, e.R(it + 0x14) | 8); }
        const uint32_t fl = e.R(it + 0x14);
        if ((fl & 0x408) || !(fl & 0x1c000)) continue;
        const uint32_t kind = e.R(e.R(it + 0x18));
        const uint32_t cnt = e.R(B + 0xD0A70 + kind * 4);
        const uint32_t tab = B + ((e.R(it + 0x24) & 1) ? 0xDEEF8u : 0xDED18u) + kind * 0x30;
        for (uint32_t i = 0; i < 8; ++i) col[i] = 0;
        for (uint32_t i = 0; i < cnt; ++i) {
            uint32_t b = 0;   // bytes: level, b, g, r as laid out on the retail stack (+0x10..+0x13)
            const uint32_t pb = (uint32_t)(uintptr_t)&b;
            d2c_light_a8230((int32_t)(e.R(n + 4) + e.R(tab + i * 8) * 8), (int32_t)(e.R(n + 8) + e.R(tab + i * 8 + 4) * 8),
                            pb, pb + 3, pb + 2, pb + 1, e);
            const uint32_t lv = b & 0xff, bl = b >> 8 & 0xff, gr = b >> 16 & 0xff, rd = b >> 24;
            if (i < 8) col[i] = lv | rd << 8 | gr << 16 | bl << 24;
        }
        uint32_t x, y;
        if (mode) { x = e.R(n + 4); y = e.R(n + 8); }
        else { x = e.R(view + 4) - e.R(view + 0x24) + e.R(it); y = e.R(it + 4) - e.R(view + 0x28) + e.R(view + 8); }
        const uint32_t a = e.R(it + 0x28) & 0xff, cel = e.R(it + 0x18), cp = (uint32_t)(uintptr_t)col;
        uint32_t r;
        if (a == 0xff) {
            r = e.H(0xd0d2, cel, x, y, cp, light);
            if (e.blk) e.blk(e.ctx, 0xd0d2, cp, 8);
        } else {
            if (e.xs) e.xs(e.ctx, a, 0);
            r = e.H(0xd0d8, cel, x, y, cp, light);
            const uint32_t s[6] = { cel, x, y, cp, light, a };
            if (e.blk) e.blk(e.ctx, 0xd0d8, (uint32_t)(uintptr_t)s, 6);
        }
        if (r) e.W(it + 0x14, e.R(it + 0x14) | 0x20000);
    }
}
