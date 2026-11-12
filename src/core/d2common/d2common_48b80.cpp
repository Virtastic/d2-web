// 1.13c D2Common+0x48B80 -- ECX, EDX and seven stack arguments, ret 28 (second stack argument = property record): draws a value between the record bounds at +8 and +0xC through 0x464D0 (EAX = max, ECX = min, EDX = the first stack argument), then returns what 0x47EF0 answers with EAX = 0, ECX = the first stack argument, EDI = the third, and the stack (ECX value, EDX value, the drawn value, fourth, fifth, sixth stack argument).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f48b80(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    (void)s6; (void)s2;
    const uint32_t v = e.h(0x464d0, e.rd32(s1 + 0xc), e.rd32(s1 + 8), s0);
    const uint32_t r = e.h(0x47ef0, 0, s0, 0, 0, 0, c, d, v, s3);
    const uint32_t w[2] = { s4, s5 };
    e.x(0x47ef0, w, 2);
    return r;
}
}
