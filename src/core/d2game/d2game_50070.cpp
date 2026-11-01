// 1.13c D2Game+0x50070: eax, esi, ebx registers and two stack words (ret 8): 0x4E2A0(eax = esi; a0, eax, ebx), 0x4F970 with six stack words, then 0x4E180(eax = esi).
#include "d2game_seam4e.h"
extern "C" void d2game_50070(uint32_t eax, uint32_t esi, uint32_t ebx, uint32_t a0, uint32_t a1, const D2Seam* e)
{
    
    e->call(0x4E2A0, esi, 0, 0, 0, 0, a0, eax, ebx);
    e->call(0x4F970, 0, 0, 0, 0, 0, a0, esi, eax, a1, ebx);
    e->call(0x4E180, esi);
}
