// 1.13c D2Game+0xAB360: AB360 item-target action handler.
// row = a0[0x1C], rec = a0[0]. it = (unit type 1, unit+0x14, [+0x28], [[+0x28]+0x28] all nonzero) ? E03A0(eax = x[0x30], ecx = x[0x28], edx = x[0x2C]) : 0. it == 0: F22C0(game; esi = unit; 0x19) and return. cnt = AB86(unit, it); cnt >= short row[idx*2+0x6E] (idx = game[0x6D]): 1CAE0(eax = unit; game, it, 3, 0, 0, 6) nonzero returns. A808(A478(unit)) nonzero: 1E140(game, unit, 0, 0, 0, 6) nonzero returns, else F22C0(short row[idx*2+0x62]) and return. it3 = B1BC0(unit, game; ebx = &loc10, &loc14); loc14 >= short row[idx*2+0x5C] zeroes it3. 1E140(game, unit, it3, loc10, 0, 6) nonzero returns. it3 == 0: F22C0(short row[idx*2+0x62]). A68E(it3, 2) nonzero or A42A(it3, 0x2D, 0) == 0x64: F2D80(eax = it3; game, unit, byte row[idx*2+0x68], 0) and return. loc10 == 0: F2540(edx = unit, ecx = game; it3, 2, 0, 0, 1, 7). Else short row+0x170 < 0 or short row[idx*2+0x56] + rec[0x18] >= unit[0xA8]: F22C0(short row[idx*2+0x62]); else F2380(al = row[0x180], ecx = unit, ebx = game; short row+0x170, it3, 0, 0) and rec[0x18] = unit[0xA8].
#include "d2game_seam4e.h"
extern "C" void d2g_ab360(uint32_t game, uint32_t unit, uint32_t a0, const D2Seam* e)
{
    uint32_t loc10 = 0, loc14 = 0;
    const uint32_t row = d2rd32(a0 + 0x1C);
    const uint32_t rec = d2rd32(a0);
    const uint32_t idx = d2rd8(game + 0x6D);
    uint32_t it = 0;
    if (unit && d2rd32(unit) == 1) {
        const uint32_t pd = d2rd32(unit + 0x14);
        const uint32_t x = pd ? d2rd32(pd + 0x28) : 0;
        const uint32_t y = x ? d2rd32(x + 0x28) : 0;
        if (y) it = e->call(0xE03A0, d2rd32(x + 0x30), y, d2rd32(x + 0x2C));
    }
    const uint32_t fin = (uint32_t)(int32_t)(int16_t)d2rd16(row + idx * 2 + 0x62);
    if (!it) { e->call(0xF22C0, game, 0, 0, 0, unit, 0x19); return; }
    const int32_t cnt = (int32_t)e->call(0xAB86, 0, 0, 0, 0, 0, unit, it);
    if (cnt >= (int16_t)d2rd16(row + idx * 2 + 0x6E)) {
        if (e->call(0x1CAE0, unit, 0, 0, 0, 0, game, it, 3, 0, 0)) return;
    }
    if (e->call(0xA808, 0, 0, 0, 0, 0, e->call(0xA478, 0, 0, 0, 0, 0, unit))) {
        if (e->call(0x1E140, 0, 0, 0, 0, 0, game, unit, 0, 0, 0)) return;
        e->call(0xF22C0, game, 0, 0, 0, unit, fin);
        return;
    }
    uint32_t it3 = e->call(0xB1BC0, unit, game, 0, (uint32_t)(uintptr_t)&loc10, 0, (uint32_t)(uintptr_t)&loc14);
    if ((int32_t)loc14 >= (int16_t)d2rd16(row + idx * 2 + 0x5C)) it3 = 0;
    if (e->call(0x1E140, 0, 0, 0, 0, 0, game, unit, it3, loc10, 0)) return;
    if (it3) {
        if (e->call(0xA68E, 0, 0, 0, 0, 0, it3, 2) || e->call(0xA42A, 0, 0, 0, 0, 0, it3, 0x2D, 0) == 0x64) {
            e->call(0xF2D80, it3, 0, 0, 0, 0, game, unit, d2rd8(row + idx * 2 + 0x68), 0);
            return;
        }
        if (!loc10) {
            e->call(0xF2540, 0, game, unit, 0, 0, it3, 2, 0, 0, 1);
            return;
        }
        const int32_t s170 = (int16_t)d2rd16(row + 0x170);
        if (s170 >= 0 && (int32_t)((int16_t)d2rd16(row + idx * 2 + 0x56) + d2rd32(rec + 0x18)) < (int32_t)d2rd32(unit + 0xA8)) {
            e->call(0xF2380, 0, unit, 0, game, 0, (uint32_t)s170, it3, 0, 0);
            d2wr32(rec + 0x18, d2rd32(unit + 0xA8));
            return;
        }
    }
    e->call(0xF22C0, game, 0, 0, 0, unit, fin);
}
