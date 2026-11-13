// 1.13c D2Game+0x155C0: AI skill pick. Fastcall ECX = game (unused), EDX = unit, one stack word `ai` (ret 4). Steps the unit seed pair
// (+0x20 low, +0x24 high: lo*0x6AC690C5 + hi, 64 bit), then tries two table slots starting at the one the new low bit selects (slot
// = (lo & 1) ^ 1, then the other). A slot is {2, 0} or {4, 0}: its word indexes the int16 skill list at ai[+0x1C] + 0x170, and
// 0xA3D0(unit, skill, -1) must answer nonzero. The first hit writes the slot pair to ai[0] +0x18 / +0x1C; none leaves both 0.
#include "d2game_seam4e.h"
extern "C" void d2g_155c0(uint32_t game, uint32_t unit, uint32_t ai, const D2Seam* e)
{
    (void)game;
    static const uint32_t slots[2][2] = { { 2, 0 }, { 4, 0 } };
    const uint64_t m = (uint64_t)d2rd32(unit + 0x20) * 0x6AC690C5u + d2rd32(unit + 0x24);
    d2wr32(unit + 0x20, (uint32_t)m);
    d2wr32(unit + 0x24, (uint32_t)(m >> 32));
    uint32_t idx = (uint32_t)m & 1;
    for (int i = 0; i < 2; i++) {
        idx = (idx + 1) & 1;
        const int16_t sk = (int16_t)d2rd16(d2rd32(ai + 0x1C) + slots[idx][0] * 2 + 0x170);
        const uint32_t r = e->call(0xA3D0, 0, unit, (uint32_t)(int32_t)sk, 0, 0, 0xFFFFFFFFu);
        if (r) {
            d2wr32(d2rd32(ai) + 0x18, slots[idx][0]);
            d2wr32(d2rd32(ai) + 0x1C, slots[idx][1]);
            return;
        }
    }
    d2wr32(d2rd32(ai) + 0x18, 0);
    d2wr32(d2rd32(ai) + 0x1C, 0);
}
