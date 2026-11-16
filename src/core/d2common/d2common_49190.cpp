// 1.13c D2Common+0x49190 -- ECX, EDX and seven stack arguments, ret 28: forwards to 0x47FA0 with EAX = 0, ECX = the second stack argument, EDX = the first, EDI = the third, and the stack (ECX value, EDX value, fourth, fifth, sixth stack argument). Returns its answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2common {
uint32_t f49190(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6)
{
    (void)s2; (void)s6;   // s2 rides in EDI, which the hook record does not carry
    const uint32_t r = e.h(0x47fa0, 0, s1, s0, 0, 0, c, d, s3, s4);
    const uint32_t w[1] = { s5 };
    e.x(0x47fa0, w, 1);
    return r;
}
}
