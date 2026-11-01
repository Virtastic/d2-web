// 1.13c D2Game+0x500A0: eax = skill id, edx = unit, one stack word (ret 4). The skill record picks a kind from its +0x82 word, a player
// unit contributes its +0x14 block's +0x28 word, and the pair goes to the 0x9D230 sender (ecx = the stack word).
#include "d2game_seam4e.h"
extern "C" void d2game_500a0(uint32_t sid, uint32_t unit, uint32_t a0, uint32_t dt, const D2Seam* e)
{
    if ((int32_t)sid < 0 || (int32_t)sid >= (int32_t)d2rd32(dt + 0xBA0)) return;
    const uint32_t rec = sid * 0x23C + d2rd32(dt + 0xB98);
    if (!rec) return;
    const int32_t w = (int16_t)d2rd16(rec + 0x82);
    const uint32_t kind = w == 0x17 ? 0xA : w == 0x1B ? 0xC : w == 0x38 ? 0xB : 0;
    uint32_t aux = 0;
    if (unit && d2rd32(unit) == 1) { const uint32_t pd = d2rd32(unit + 0x14); if (pd) aux = d2rd32(pd + 0x28); }
    e->call(0x9D230, 0, a0, unit, 0, 0, aux, kind);
}
