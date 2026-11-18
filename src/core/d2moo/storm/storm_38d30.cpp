// 1.13c Storm+0x38D30 -- stdcall, one argument = buffer: calls the worker 0x3B080 with (buffer, the module string at image offset 0x46870, 0x7FFFFFFF), ESI = buffer, and returns buffer + its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace stm {
uint32_t f38d30(GE& e, uint32_t a)
{
    return e.h(0x3B080, 0, 0, 0, 0, a, a, 0xB2046870u, 0x7fffffff) + a;
}
}
