// 1.13c D2Game+0xA7DC0: A7DC0 item-use action handler.
// row = a0[0x1C]; s170 = short row+0x170; lim = short row[game[0x6D]*2+0x68]. A4A10(unit; game, a0, 0) nonzero returns. v = A3F4(unit, s170). v == 0: A496(unit, 0); 75E10(game, &blk, 1) with blk = 8 dwords, [1] = unit, byte 0x15 = 0x64; return. Else if A68E(unit, 0xC) A3CA(unit, 0xC, 0). it3 = B1BC0(unit, game; &loc) (loc written). it3 == 0 or loc >= lim: F22C0(short row[idx*2+0x62]). Else rand(100) 1180 >= short row[idx*2+0x56]: F22C0(short row[idx*2+0x5C]). Else A4A10(unit; game, a0, 1) nonzero returns; 8E6F0 per type-2 node of unit+0xDC; r = AE1A (D2Common #10136, one stack word: v ? v[8] : 0)(v); F2380(al r, unit, game; s170, it3, 0, 0).
#include "d2game_seam4e.h"
extern "C" void d2g_a7dc0(uint32_t game, uint32_t unit, uint32_t a0, const D2Seam* e)
{
    uint32_t loc = 0, ebxout = 0;
    const uint32_t row = d2rd32(a0 + 0x1C);
    const int32_t s170 = (int16_t)d2rd16(row + 0x170);
    const int32_t lim = (int16_t)d2rd16(row + d2rd8(game + 0x6D) * 2 + 0x68);
    if (e->call(0xA4A10, unit, 0, 0, 0, 0, game, a0, 0)) return;
    const uint32_t v = e->call(0xA3F4, 0, unit, (uint32_t)s170);
    if (!v) {
        e->call(0xA496, 0, 0, 0, 0, 0, unit, 0);
        uint32_t blk[8] = { 0, unit, 0, 0, 0, 0, 0, 0 };
        ((uint8_t*)blk)[0x15] = 0x64;
        e->call(0x75E10, 0, 0, 0, 0, 0, game, (uint32_t)(uintptr_t)blk, 1);
        e->blk(0x75E10, blk, 8);
        return;
    }
    if (e->call(0xA68E, 0, 0, 0, 0, 0, unit, 0xC)) e->call(0xA3CA, 0, 0, 0, 0, 0, unit, 0xC, 0);
    const uint32_t it3 = e->call(0xB1BC0, unit, game, 0, (uint32_t)(uintptr_t)&ebxout, 0, (uint32_t)(uintptr_t)&loc);
    uint32_t idx = d2rd8(game + 0x6D);
    if (!it3 || (int32_t)loc >= lim) { e->call(0xF22C0, game, 0, 0, 0, 0, (uint32_t)(int32_t)(int16_t)d2rd16(row + idx * 2 + 0x62)); return; }
    const int32_t r = (int32_t)e->call(0x1180, 100, unit + 0x20);
    if (r >= (int16_t)d2rd16(row + idx * 2 + 0x56)) { e->call(0xF22C0, game, 0, 0, 0, 0, (uint32_t)(int32_t)(int16_t)d2rd16(row + idx * 2 + 0x5C)); return; }
    if (e->call(0xA4A10, unit, 0, 0, 0, 0, game, a0, 1)) return;
    for (uint32_t n = d2rd32(unit + 0xDC); n;) {
        const uint32_t nx = d2rd32(n + 0x24);
        if (d2rd8(n) == 2) e->call(0x8E6F0, n);
        n = nx;
    }
    const uint32_t ae = e->call(0xAE1A, 0, 0, 0, 0, 0, v);
    e->call(0xF2380, ae, unit, 0, game, 0, (uint32_t)s170, it3, 0, 0);
}
