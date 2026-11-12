// 1.13c D2Common+0x7BE10 -- state pick (ecx = the record, ebx carried): when the state byte at +0x44 is below 4 and its word at +0xC[state] is not zero the record is returned untouched; otherwise the first of the four words that is not zero (4 when all are zero) is handed to the worker 0x7BB70 in BL (eax = the record) and stored as the new state byte of the record it answers.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
static uint32_t rd16(GE& e, uint32_t a) { return e.rd8(a) | (e.rd8(a + 1) << 8); }
uint32_t f7be10(GE& e, uint32_t u, uint32_t bx)
{
    const uint32_t c = e.rd8(u + 0x44);
    if (c < 4 && rd16(e, u + 0xc + c * 2)) return u;
    uint32_t bl = 0;
    while (bl < 4 && !rd16(e, u + 0xc + bl * 2)) bl++;
    const uint32_t r = e.h(0x7bb70, u, 0, 0, (bx & 0xffffff00u) | bl, 0);
    e.wr8(r + 0x44, bl);
    return r;
}
}
