// 1.13c D2Game+0xBDE50 -- ECX = a, EDX = b, one stack argument c: calls 0xBD9B0 (EAX = c, ECX = b, one stack word = a) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fbde50(GE& e, uint32_t a, uint32_t b, uint32_t c)
{
    return e.h(0xBD9B0, c, b, 0, 0, 0, a);
}
}
