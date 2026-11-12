// 1.13c D2Common+0x47B20 -- ECX, EDX and seven stack arguments, ret 28 (second stack argument = property record with parameter +4, min +8, max +0xC): min above max, an unresolved stat list (0x47020), a parameter outside 0..3 or a bound outside -0x100..0x2FF are fatal (lines 0x444, 0x44A, 0x44C, 0x44D, 0x44E). Otherwise writes the stat named by the third stack argument through 0x3A280 with the packed value parameter + 4 * ((max + 0x100) * 1024 + min + 0x100) and layer 0. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f47b20(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    auto fatal = [&](uint32_t line) { const uint32_t v = e.h(0x921c); e.h(0x9216, 0, 0, 0, 0, 0, 0xB208a728u, v, line); e.h(0x1b0d, 0, 0, 0, 0, 0, 0xffffffffu); };
    (void)c; (void)s3; (void)s6;
    const int32_t lo0 = (int32_t)e.rd32(s1 + 8), hi0 = (int32_t)e.rd32(s1 + 0xc);
    if (lo0 > hi0) { fatal(0x444); return 0; }
    const int32_t par = (int32_t)e.rd32(s1 + 4);
    const uint32_t lo = (uint32_t)lo0 + 0x100, hi = (uint32_t)hi0 + 0x100;
    const uint32_t r = e.h(0x47020, s0, d, 0, 0, 0, s4, s5);
    if (!r) { fatal(0x44a); return 0; }
    if (par < 0 || par > 3) { fatal(0x44c); return 0; }
    if ((int32_t)lo < 0 || (int32_t)lo >= 0x400) { fatal(0x44d); return 0; }
    if ((int32_t)hi < 0 || (int32_t)hi >= 0x400) { fatal(0x44e); return 0; }
    e.h(0x3a280, 0, 0, 0, 0, 0, r, s2, (uint32_t)par + ((hi << 10) + lo) * 4, 0);
    return 1;
}
}
