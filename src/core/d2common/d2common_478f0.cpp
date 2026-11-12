// 1.13c D2Common+0x478F0 -- ECX, EDX and seven stack arguments, ret 28 (second stack argument = property record): draws a value between the record bounds through 0x464D0 (a zero draw returns 0), resolves the stat list through 0x47020 (EAX = first stack argument, ECX = the EDX value, stack = fifth and sixth stack arguments; none returns 0). When the fourth stack argument is set the value becomes the negated answer of 0x38B00 for stat record 0x7E of the stat cost table (0 when the table has no such record). Writes stat 0x7E through 0x3A330 (list, 0x7E, value, 1) and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_99e1c;
namespace d2common {
uint32_t f478f0(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    (void)c; (void)s2; (void)s6;
    uint32_t v = e.h(0x464d0, e.rd32(s1 + 0xc), e.rd32(s1 + 8), s0);
    if (!v) return 0;
    const uint32_t r = e.h(0x47020, s0, d, 0, 0, 0, s4, s5);
    if (!r) return 0;
    if (s3) {
        const uint32_t t = g_d2common_99e1c;
        uint32_t x = 0;
        if ((int32_t)e.rd32(t + 0xbd4) > 0x7e) {
            const uint32_t p = e.rd32(t + 0xbcc) + 0x9f78;
            if (p) {
                e.ed(0x7e0001);
                x = e.h(0x38b00, 0, 0, 0, r, 0, p);
                e.edi(0x38b00, 0x7e0001);
            }
        }
        v = 0u - x;
    }
    e.h(0x3a330, 0, 0, 0, 0, 0, r, 0x7e, v, 1);
    return 1;
}
}
