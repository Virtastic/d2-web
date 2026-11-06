// 1.13c D2Game+0x563C0 (guarded record create): EBX = game, stack (p1, p2, outPtr, p4, p5, p6), ret 0x18. When the module guard dword (RVA 0x111C3C) is non-zero,
// *outPtr = 0 and 0xE is returned with no call. Otherwise 0x56250(ebx, p1, p2, &local, p4, p5, p6) creates the record into the local and returns the status; a
// non-zero local is released through 0x35090(ebx, local, 0); *outPtr = local; the 0x56250 status is returned.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_563c0(uint32_t ebx, uint32_t p1, uint32_t p2, uint32_t outPtr, uint32_t p4, uint32_t p5, uint32_t p6, uint32_t guard, const D2Seam* e)
{
    if (guard != 0) { d2wr32(outPtr, 0); return 0xE; }
    uint32_t local = 0;
    const uint32_t w[7] = { ebx, p1, p2, (uint32_t)(uintptr_t)&local, p4, p5, p6 };
    const uint32_t st = e->call(0x56250, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6]);
    e->blk(0x56250, w, 7);
    if (local != 0) e->call(0x35090, 0, 0, 0, 0, 0, ebx, local, 0);
    d2wr32(outPtr, local);
    return st;
}
