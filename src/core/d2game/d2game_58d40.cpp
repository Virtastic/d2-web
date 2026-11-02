// 1.13c D2Game+0x58D40: ecx = unit (+0x6D byte index), edx = ctx, one unused stack word... (ret 4: the stack word is the unit passed on); returns 0. #A4C0 = Ordinal_10920(ctx); #A568 = Ordinal_10174 (getter); #354F0 is a register callee (al = 0x24 over the last getter result's upper bytes, ecx = ctx, one stack word = unit).
#include "d2game_seam4e.h"
extern "C" void d2g_58d40(uint32_t unit, uint32_t ctx, const D2Seam* e)
{
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, ctx);
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(unit + 0x6D) * 4);
    uint32_t last = e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x28, 0);
    bool go = false;
    if (!last) { last = e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x28, 0xF); go = (last == 0); }
    if (!go) {
        last = e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x28, 0xD);
        if (last) go = true;
        else { last = e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x28, 0xE); go = (last != 0); }
    }
    if (go) e->call(0x354F0, (last & ~0xFFu) | 0x24, ctx, 0, 0, 0, unit);
}
