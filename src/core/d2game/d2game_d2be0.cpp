// D2Game+0xD2BE0 -- EAX = unit, EDI = record pointer or 0, EBX = value (full register), plain ret. Written from the 1.13c disassembly; callees are the D2Seam.
// ebp = 0xD1AA0(EAX = unit); -1 ends. 0xA42A(unit, 0x46, 0); esi = 0xA3D0(ECX = EDI, EDX = ebp, stack -1); esi != 0: 0xA8E0(esi);
// 0xA862(esi, 0, EBX) leaves EAX residue. Then 0x8D570(AL = BL, ECX = ebp, EDX = tmp, EBX, ESI, stack): EDI == 0: BL = 6, tmp = -1, ESI 0, stack 0;
// else EBX = [EDI] and tmp = [EDI + 0xC]: EBX != 0 gives ESI 0, stack 0; EBX == 0 gives ESI = [0xA4C0(EDI) + 0x9C] (EAX = that call's result), stack 0.
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD2BE0
extern "C" void d2game_d2be0(uint32_t unit, uint32_t rec, uint32_t ebx, const D2Seam* e)
{
    const uint32_t ebp = e->call(0xD1AA0, unit);
    if (ebp == 0xFFFFFFFFu) return;
    e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0x46, 0);
    uint32_t esi = e->call(0xA3D0, 0, rec, ebp, 0, 0, 0xFFFFFFFFu);
    if (esi) e->call(0xA8E0, 0, 0, 0, 0, 0, esi);
    uint32_t eax = e->call(0xA862, 0, 0, 0, 0, 0, esi, 0, ebx);
    uint32_t tmp;
    esi = 0;
    if (!rec) { tmp = 0xFFFFFFFFu; ebx = 6; }
    else {
        ebx = d2rd32(rec); tmp = d2rd32(rec + 0xC);
        if (!ebx) {
            eax = e->call(0xA4C0, 0, 0, 0, 0, 0, rec);
            esi = d2rd32(eax + 0x9C);
        }
    }
    eax = (eax & ~0xFFu) | (ebx & 0xFF);
    e->call(0x8D570, eax, ebp, tmp, ebx, esi, 0);
}
