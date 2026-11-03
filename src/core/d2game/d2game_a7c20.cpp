// 1.13c D2Game+0xA7C20: A7C20 item-stack/convert action handler.
// r = A4A10(unit; game, a0, 0) nonzero returns. row = a0[0x1C]; s170 = short row+0x170; v = A3F4(unit, s170) nonzero; cnt = A400(unit, v, 1) > 0; it3 = B1BC0(unit, game; &loc) (loc written); it3 == 0: F22C0(short row[game[0x6D]*2+0x5C]). ip = 9E810(unit, cnt); fld = [[a0]+0x14]; cap = 1A40(s170, cnt); half = (cap - (cap>>31)) >> 1. If ip != 0, ip[0xC] != fld and AB86(it3, ip) < half: A4A10(unit; game, a0, 1) nonzero returns; [[a0]+0x14] = ip[0xC]; F2380(al 9, unit, game; s170, ip, 0, 0). Else if loc < short row[idx*2+0x68] and rand(100) 1180 < short row[idx*2+0x62]: A4A10(unit; game, a0, 1) nonzero returns, F2380(al 0xE; short row+0x172, it3, 0, 0); else F22C0(short row[idx*2+0x5C]).
#include "d2game_seam4e.h"
extern "C" void d2g_a7c20(uint32_t game, uint32_t unit, uint32_t a0, const D2Seam* e)
{
    uint32_t loc = 0, ebxout = 0;
    if (e->call(0xA4A10, unit, 0, 0, 0, 0, game, a0, 0)) return;
    const uint32_t row = d2rd32(a0 + 0x1C);
    const int32_t s170 = (int16_t)d2rd16(row + 0x170);
    const uint32_t v = e->call(0xA3F4, 0, unit, (uint32_t)s170);
    if (!v) return;
    const int32_t cnt = (int32_t)e->call(0xA400, 0, 0, 0, 0, 0, unit, v, 1);
    if (cnt <= 0) return;
    const uint32_t it3 = e->call(0xB1BC0, unit, game, 0, (uint32_t)(uintptr_t)&ebxout, 0, (uint32_t)(uintptr_t)&loc);
    const uint32_t idx = d2rd8(game + 0x6D);
    if (!it3) { e->call(0xF22C0, game, 0, 0, 0, 0, (uint32_t)(int32_t)(int16_t)d2rd16(row + idx * 2 + 0x5C)); return; }
    const uint32_t ip = e->call(0x9E810, 0, 0, 0, 0, 0, unit, (uint32_t)cnt);
    const uint32_t fld = d2rd32(d2rd32(a0) + 0x14);
    const int32_t cap = (int32_t)e->call(0x1A40, (uint32_t)s170, 0, (uint32_t)cnt);
    const int32_t half = (cap - (cap >> 31)) >> 1;
    bool alt = !ip || d2rd32(ip + 0xC) == fld;
    if (!alt && (int32_t)e->call(0xAB86, 0, 0, 0, 0, 0, it3, ip) >= half) alt = true;
    if (!alt) {
        if (e->call(0xA4A10, unit, 0, 0, 0, 0, game, a0, 1)) return;
        d2wr32(d2rd32(a0) + 0x14, d2rd32(ip + 0xC));
        e->call(0xF2380, (d2rd32(ip + 0xC) & ~0xFFu) | 9, unit, 0, game, 0, (uint32_t)s170, ip, 0, 0);
        return;
    }
    if ((int32_t)loc < (int16_t)d2rd16(row + idx * 2 + 0x68)) {
        const int32_t r = (int32_t)e->call(0x1180, 100, unit + 0x20);
        if (r < (int16_t)d2rd16(row + idx * 2 + 0x62)) {
            if (e->call(0xA4A10, unit, 0, 0, 0, 0, game, a0, 1)) return;
            e->call(0xF2380, 0xE, unit, 0, game, 0, (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x172), it3, 0, 0);
            return;
        }
    }
    e->call(0xF22C0, game, 0, 0, 0, 0, (uint32_t)(int32_t)(int16_t)d2rd16(row + idx * 2 + 0x5C));
}
