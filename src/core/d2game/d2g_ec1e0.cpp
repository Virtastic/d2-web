// 1.13c D2Game+0xEC1E0: pet item section ('fj') of the save writer, callees through the D2Seam recorder.
// EAX = game; stack (ret 0x14): a1 (pet lookup owner), &cursor, end, a4, a5.
// 0x96A40 gets EAX = a1, EBX = 1, ESI = 7, EDI = game; 0x537D0 gets ECX = cursor, EDX = pet[0x60] and stack game, pet,
// end - cursor, 0, a4, a5.
#include "d2game_seam4e.h"
extern "C" uint32_t d2g_ec1e0(uint32_t game, uint32_t a1, uint32_t pCur, uint32_t end, uint32_t a4, uint32_t a5, const D2Seam* e)
{
    uint32_t cur = d2rd32(pCur);
    if (d2rd32(game + 0x70) == 0) return 0;
    d2wr16(cur, 0x666a);
    cur += 2;
    const uint32_t pet = e->call(0x96A40, a1, 0, 0, 1, 7);
    e->edi(0x96A40, game);
    if (pet) {
        const int32_t n = (int32_t)e->call(0x537D0, 0, cur, d2rd32(pet + 0x60), 0, 0, game, pet, end - cur, 0, a4, a5);
        if (n < 0) return 5;
        cur += (uint32_t)n;
    }
    d2wr32(pCur, cur);
    return 0;
}
