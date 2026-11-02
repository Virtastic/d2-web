// 1.13c D2Game+0x4E180 (StatList_AddFourDerivedStats): eax = unit, edi = stat spec (+0 ecx field, +8 eax field, +0xC edx field; or null)
// (otherwise null). Calls: #10973 (thunk 0xA42A), #11013 (0xA6A0, 5 stack words), #10807 (0xA60A), #10188 (0xA6E2). esi is only saved and clobbered.
#include "d2game_seam4e.h"
extern "C" void d2game_4e180(uint32_t unit, uint32_t edi, const D2Seam* e)
{
    const uint32_t ebx = e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0x15d, 0);
    if (!ebx) return;
    uint32_t eax, ecx, edx;
    if (!edi) { edx = 0xffffffffu; ecx = 6; eax = 0; } else { edx = d2rd32(edi + 0xC); ecx = d2rd32(edi); eax = d2rd32(edi + 8); }
    const uint32_t st = e->call(0xA6A0, 0, 0, 0, 0, 0, eax, 0, 0, ecx, edx);
    if (!st) return;
    e->call(0xA60A, 0, 0, 0, 0, 0, edi, st, 1);
    static const uint32_t ids[3] = { 0x8e, 0x90, 0x94 }, out[3] = { 0x27, 0x29, 0x2b };
    for (int i = 0; i < 3; ++i)
        if ((int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, edi, ids[i], 0) <= 0) e->call(0xA6E2, 0, 0, 0, 0, 0, st, out[i], ebx, 0);
    e->call(0xA6E2, 0, 0, 0, 0, 0, st, 0x2d, ebx, 0);
}
