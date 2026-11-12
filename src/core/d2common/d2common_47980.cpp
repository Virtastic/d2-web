// 1.13c D2Common+0x47980 -- ECX, EDX and seven stack arguments, ret 28 (second stack argument = property record, NULL returns 0): draws a value between the record bounds through 0x464D0; the record parameter at +4 must be a valid index below the count at +0xBA0 of the data tables and the stat list (0x47020) must resolve, otherwise returns 0. When the fourth stack argument is set the value becomes the negated answer of 0x394E0 (EAX = 0x6B, ECX = list, stack = the parameter). Writes stat 0x6B with that parameter through 0x3A330 and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2common_99e1c;
namespace d2common {
uint32_t f47980(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    (void)c; (void)s2; (void)s6;
    if (!s1) return 0;
    uint32_t v = e.h(0x464d0, e.rd32(s1 + 0xc), e.rd32(s1 + 8), s0);
    const int32_t id = (int32_t)e.rd32(s1 + 4);
    if (id < 0 || id >= (int32_t)e.rd32(g_d2common_99e1c + 0xba0)) return 0;
    const uint32_t r = e.h(0x47020, s0, d, 0, 0, 0, s4, s5);
    if (!r) return 0;
    if (s3) v = 0u - e.h(0x394e0, 0x6b, r, 0, 0, 0, (uint32_t)id);
    e.h(0x3a330, 0, 0, 0, 0, 0, r, 0x6b, v, (uint32_t)id);
    return 1;
}
}
