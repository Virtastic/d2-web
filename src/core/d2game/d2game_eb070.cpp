// 1.13c D2Game+0xEB070 -- ECX = a, EDX = b, one stack argument c: calls 0x8D120 (EAX = c, ECX = a, EBX = b) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t feb070(GE& e, uint32_t a, uint32_t b, uint32_t c)
{
    return e.h(0x8D120, c, a, 0, b, 0);
}
}
