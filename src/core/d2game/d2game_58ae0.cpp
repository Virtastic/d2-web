// 1.13c D2Game+0x58AE0: ecx = object g (+0x6D byte index), edx = arg unit, one unused stack word (ret 4); returns 0. #A4C0 = Ordinal_10920(arg); #A568 = Ordinal_10174 (getter); #A4DE = Ordinal_10158. A null arg is a retail _exit(-1) path and is not reproduced.
#include "d2game_seam4e.h"
extern "C" void d2g_58ae0(uint32_t g, uint32_t arg, const D2Seam* e)
{
    const uint32_t tbl = e->call(0xA4C0, 0, 0, 0, 0, 0, arg);
    const uint32_t ent = d2rd32(tbl + 0x10 + d2rd8(g + 0x6D) * 4);
    if (!e->call(0xA568, 0, 0, 0, 0, 0, ent, 0x28, 0xD)) return;
    if (arg && d2rd32(arg) == 0) e->call(0xA4C0, 0, 0, 0, 0, 0, arg);
    uint32_t pk[2] = { 0x5Du | (0x24u << 8) | (0x02u << 16), 0 };
    e->call(0x8A3E0, 0, 0, 0, 0, 0, (uint32_t)(uintptr_t)pk, 6);
    e->blk(0x8A3E0, pk, 2);
    d2wr16(arg + 0x6E, 0x53);
    d2wr32(arg + 0x70, arg);
    e->call(0xA4DE, 0, 0, 0, 0, 0, arg);
    d2wr32(arg + 0xC4, d2rd32(arg + 0xC4) | 0x400);
}
