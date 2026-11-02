// 1.13c D2Game+0x4C220 (sub_4C220): ecx = struct (+4 dword, +0xC pointer q), edx = value. Copies a 9-dword block onto the stack and
// hands it to 0xA00B0 in EAX; returns 1.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4c220(uint32_t s, uint32_t val, const D2Seam* e)
{
    const uint32_t q = d2rd32(s + 0xC);
    uint32_t b[9] = { d2rd32(s + 4), val, d2rd32(q), d2rd32(q + 4), d2rd32(q + 8), d2rd32(q + 0x10), d2rd32(q + 0x14), d2rd32(q + 0xC), 0 };
    e->call(0xA00B0, (uint32_t)(uintptr_t)b);
    e->blk(0xA00B0, b, 9);
    return 1;
}
