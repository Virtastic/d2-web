// 1.13c D2Common+0x47E20 -- ECX = mode, EDX and seven stack arguments, ret 28 (second stack argument = property record, NULL fatal at line 0x2E2; third = stat id): a zero record parameter (+4) returns 0. For stats 0x10..0x12, or in mode 1, 0x46E50 is called with the stat id; the value goes through the scaler 0x45640 (EAX = stat id, ECX = the value in place); the stat list comes from 0x47020 (none is fatal, line 0x2F0). When the fourth stack argument is set the value becomes the negated answer of 0x39140 (list, stat, 0). Writes the stat through 0x3A330 (list, stat, value, 0) and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f47e20(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    auto fatal = [&](uint32_t line) { const uint32_t v = e.h(0x921c); e.h(0x9216, 0, 0, 0, 0, 0, 0xB208a728u, v, line); e.h(0x1b0d, 0, 0, 0, 0, 0, 0xffffffffu); };
    (void)s6;
    if (!s1) { fatal(0x2e2); return 0; }
    uint32_t v = e.rd32(s1 + 4);
    if (!v) return 0;
    if (((int32_t)s2 >= 0x10 && (int32_t)s2 <= 0x12) || c == 1) e.h(0x46e50, 0, 0, 0, 0, 0, s2);
    e.h(0x45640, s2, e.stk(&v));
    const uint32_t r = e.h(0x47020, s0, d, 0, 0, 0, s4, s5);
    if (!r) { fatal(0x2f0); return 0; }
    if (s3) v = 0u - e.h(0x39140, 0, 0, 0, 0, 0, r, s2, 0);
    e.h(0x3a330, 0, 0, 0, 0, 0, r, s2, v, 0);
    return 1;
}
}
