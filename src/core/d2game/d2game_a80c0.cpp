// 1.13c D2Game+0xA80C0: A80C0 cooldown-timed action handler.
// ct = (unit type 1 and unit+0x14) ? [unit+0x14]+0x30 : 0; any node of the list *ct (next at +8) with a nonzero first dword: F22C0(game; 0xA) and return. p = F30C0(unit; game, &out) (out written). If p != 0, p[0] == 0, 37680(p) == 0, rand(3) 1180 != 0 and out != 0: rec = *a0; rec[0x18] == 0: rec[0x18] = 0x3C and DFFD0(unit; 0x12, p); rec[0x18] nonzero: negative becomes 0, then a positive one is decremented; F22C0(game; 0x14). Else it3 = B1BC0(unit, game; &loc) (loc written): it3 == 0 or loc >= 0x19: F22C0(game; 0x32). Else A496(unit, 0); blk[8] = {4, unit, it3, 0...}, byte blk+0x15 = 0x64, or 0x65 when row = 1320(unit[4]) is nonzero and row byte +0x104 & bm[4]; 75E10(game, &blk, 1).
#include "d2game_seam4e.h"
extern "C" void d2g_a80c0(uint32_t game, uint32_t unit, uint32_t a0, uint32_t bm, const D2Seam* e)
{
    uint32_t out = 0, loc = 0, ebxout = 0;
    uint32_t ct = 0;
    if (unit && d2rd32(unit) == 1 && d2rd32(unit + 0x14)) ct = d2rd32(d2rd32(unit + 0x14) + 0x30);
    for (uint32_t n = d2rd32(ct); n; n = d2rd32(n + 8))
        if (d2rd32(n)) { e->call(0xF22C0, game, 0, 0, 0, 0, 0xA); return; }
    const uint32_t p = e->call(0xF30C0, 0, 0, 0, 0, unit, game, (uint32_t)(uintptr_t)&out);
    if (p && !d2rd32(p) && !e->call(0x37680, 0, 0, 0, 0, p) && e->call(0x1180, 3, unit + 0x20) && out) {
        const uint32_t rec = d2rd32(a0);
        int32_t cnt = (int32_t)d2rd32(rec + 0x18);
        if (cnt == 0) {
            d2wr32(rec + 0x18, 0x3C);
            if (!d2rd32(p)) e->call(0xDFFD0, 0, 0, 0, 0, unit, 0x12, p);
        } else {
            if (cnt < 0) d2wr32(rec + 0x18, 0);
            cnt = (int32_t)d2rd32(rec + 0x18);
            if (cnt > 0) d2wr32(rec + 0x18, (uint32_t)(cnt - 1));
        }
        e->call(0xF22C0, game, 0, 0, 0, 0, 0x14);
        return;
    }
    const uint32_t it3 = e->call(0xB1BC0, unit, game, 0, (uint32_t)(uintptr_t)&ebxout, 0, (uint32_t)(uintptr_t)&loc);
    if (!it3 || (int32_t)loc >= 0x19) { e->call(0xF22C0, game, 0, 0, 0, 0, 0x32); return; }
    e->call(0xA496, 0, 0, 0, 0, 0, unit, 0);
    uint32_t blk[8] = { 4, unit, it3, 0, 0, 0, 0, 0 };
    uint32_t bl = 0x64;
    if (unit) {
        const uint32_t m = e->call(0x1320, d2rd32(unit + 4));
        if (m && (d2rd8(m + 0x104) & d2rd32(bm + 0x10))) bl = 0x65;
    }
    ((uint8_t*)blk)[0x15] = (uint8_t)bl;
    e->call(0x75E10, 0, 0, 0, 0, 0, game, (uint32_t)(uintptr_t)blk, 1);
    e->blk(0x75E10, blk, 8);
}
