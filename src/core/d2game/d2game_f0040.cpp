// 1.13c D2Game+0xF0040 -- ECX = c, EDX = d, one stack argument a (ret 4): calls the worker 0xEFFD0 with EAX = a, EDI = d and c on the stack (ret 4) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t ff0040(GE& e, uint32_t c, uint32_t d, uint32_t a)
{
    const uint32_t v = e.h(0xEFFD0, a, 0, 0, 0, 0, c);
    e.j(0xEFFD0, d);
    return v;
}
}
