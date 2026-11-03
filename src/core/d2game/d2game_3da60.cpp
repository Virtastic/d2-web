// 1.13c D2Game+0x3DA60: ecx = pGame, edx = missile, [esp+4] = origin unit (the missile itself when 0), ret 4. Same missile gates as 0x3C700 (record
// (int16)row[0x24] >= 0, flag bit 10, owner = #E03A0(eax = m[0x94], edx = m[0x98], ecx = game)). Then #3CB00(eax = origin, edx = (int16)row[0x24];
// stack game, owner, #B26A(m), #B0FC(m), max(row[0x4C], 1)); the two 1-argument calls leave the pushed step and level words on the stack for the
// next call's argument block, which is why the block is that order (the capture logs only the first four stack words; the step word is not recorded). Returns 3.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_3da60(uint32_t game, uint32_t m, uint32_t origin, uint32_t dt, const D2Seam* e)
{
    if (!m) return 1;
    const int32_t id = (int32_t)d2rd32(m + 4);
    if (id < 0 || id >= (int32_t)d2rd32(dt + 0xB6C)) return 1;
    const uint32_t row = id * 0x1A4 + d2rd32(dt + 0xB64);
    if (!row || (int16_t)d2rd16(row + 0x24) < 0) return 1;
    if (!(d2rd32(m + 0xC8) >> 10 & 1)) return 1;
    const uint32_t owner = e->call(0xE03A0, d2rd32(m + 0x94), game, d2rd32(m + 0x98));
    if (!owner) return 1;
    if (!origin) origin = m;
    int32_t step = (int32_t)d2rd32(row + 0x4C);
    if (step < 1) step = 1;
    const uint32_t r1 = e->call(0xB0FC, 0, 0, 0, 0, 0, m);
    const uint32_t r2 = e->call(0xB26A, 0, 0, 0, 0, 0, m);
    const uint32_t w[5] = { game, owner, r2, r1, (uint32_t)step };
    e->call(0x3CB00, origin, 0, (uint32_t)(int32_t)(int16_t)d2rd16(row + 0x24), 0, 0, w[0], w[1], w[2], w[3], w[4]);
    return 3;
}
