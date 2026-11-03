// 1.13c D2Game+0xA1600: ecx = esi, edx = ebx. #9EB80(eax = ebx) nonzero returns 1; else #79100(eax = ebx; stack esi), #78A90(stack esi), return 0.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_a1600(uint32_t esi, uint32_t ebx, const D2Seam* e)
{
    if (e->call(0x9EB80, ebx)) return 1;
    e->call(0x79100, ebx, 0, 0, 0, 0, esi);
    e->call(0x78A90, 0, 0, 0, 0, 0, esi);
    return 0;
}
