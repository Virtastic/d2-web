// 1.13c D2Game+0x96810: edx = x, stack p (ret 4). sender as in 0x966E0; a 13-byte message {0x7A, 0, byte [p +0xC], word [p +8], dword [p +4], dword [p]} goes to
// 0x8A3E0(eax = sender; &msg, 0xD). Returns the callee's eax.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_96810(uint32_t x, uint32_t p, const D2Seam* e)
{
    uint32_t sender = 0;
    if (x && d2rd32(x) == 0) sender = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, x) + 0x9C);
    uint8_t m[16] = {0};
    m[0] = 0x7A;
    m[2] = d2rd8(p + 0xC);
    const uint32_t w = d2rd16(p + 8), a = d2rd32(p + 4), b = d2rd32(p);
    __builtin_memcpy(m + 3, &w, 2);
    __builtin_memcpy(m + 5, &a, 4);
    __builtin_memcpy(m + 9, &b, 4);
    const uint32_t r = e->call(0x8A3E0, sender, 0, 0, 0, 0, (uint32_t)(uintptr_t)m, 0xD);
    e->blk(0x8A3E0, m, 4);
    return r;
}
