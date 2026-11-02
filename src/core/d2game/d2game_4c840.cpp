// 1.13c D2Game+0x4C840 (sub_4C840): ecx, edx register args, two ignored stack words (ret 8).
// 0x9EB80(eax = edx) nonzero: return 1. Else 0x79100(eax = edx, stack ecx) and 0x78A90(stack ecx), return 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_4c840(uint32_t ecx, uint32_t edx, const D2Seam* e)
{
    if (e->call(0x9EB80, edx)) return 1;
    e->call(0x79100, edx, 0, 0, 0, 0, ecx);
    e->call(0x78A90, 0, 0, 0, 0, 0, ecx);
    return 0;
}
