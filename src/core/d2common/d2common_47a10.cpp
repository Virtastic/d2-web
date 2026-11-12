// 1.13c D2Common+0x47A10 -- ECX unused, EDX and seven stack arguments, ret 28 (second stack argument = property record with parameter +4, min +8, max +0xC; third = stat id S): scales min for stat S, max for S+1 and the parameter for S+2 through the scaler 0x45640, resolves the stat list through 0x47020 (EAX = first stack argument, ECX = the EDX value, stack = fifth and sixth stack arguments; none is fatal, line 0x4AD). When the fourth stack argument is set the three values become the negated answers of 0x39140 (list, S / S+1 / S+2, 0). Writes S, S+1 and S+2 through 0x3A330 with layer 0, then stat 0x146 = 1 when S is 0x39. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
extern "C" void D2Common_45640(int32_t nSelector, int32_t* pValue);
uint32_t f47a10(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    auto fatal = [&](uint32_t line) { const uint32_t v = e.h(0x921c); e.h(0x9216, 0, 0, 0, 0, 0, 0xB208a728u, v, line); e.h(0x1b0d, 0, 0, 0, 0, 0, 0xffffffffu); };
    (void)c; (void)s6;
    int32_t lo = (int32_t)e.rd32(s1 + 8), hi = (int32_t)e.rd32(s1 + 0xc), par = (int32_t)e.rd32(s1 + 4);
    D2Common_45640((int32_t)s2, &lo);
    D2Common_45640((int32_t)(s2 + 1), &hi);
    D2Common_45640((int32_t)(s2 + 2), &par);
    const uint32_t r = e.h(0x47020, s0, d, 0, 0, 0, s4, s5);
    if (!r) { fatal(0x4ad); return 0; }
    uint32_t a = (uint32_t)lo, b = (uint32_t)hi, p = (uint32_t)par;
    if (s3) {
        a = 0u - e.h(0x39140, 0, 0, 0, 0, 0, r, s2, 0);
        b = 0u - e.h(0x39140, 0, 0, 0, 0, 0, r, s2 + 1, 0);
        p = 0u - e.h(0x39140, 0, 0, 0, 0, 0, r, s2 + 2, 0);
    }
    e.h(0x3a330, 0, 0, 0, 0, 0, r, s2, a, 0);
    e.h(0x3a330, 0, 0, 0, 0, 0, r, s2 + 1, b, 0);
    e.h(0x3a330, 0, 0, 0, 0, 0, r, s2 + 2, p, 0);
    if (s2 == 0x39) e.h(0x3a330, 0, 0, 0, 0, 0, r, 0x146, 1, 0);
    return 1;
}
}
