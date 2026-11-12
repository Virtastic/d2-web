// 1.13c D2Net+0x63C0 -- ECX passes through, EDX = value, one stack argument: calls the worker 0x6BD0 with (value, argument) and returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2n {
uint32_t f63c0(GE& e, uint32_t c, uint32_t d, uint32_t a)
{
    e.h(0x6BD0, 0, c, 0, 0, 0, d, a);
    return 1;
}
}
