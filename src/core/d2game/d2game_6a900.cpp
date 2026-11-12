// 1.13c D2Game+0x6A900 -- ECX = unit, EDX passes through, one stack argument: the worker 0x9F7A0 runs with the argument on the stack and in EAX, ECX = unit; then the D2Common import 0xA460(unit) gives a value handed to the import 0xA472(unit, 0xA, value, 0), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f6a900(GE& e, uint32_t unit, uint32_t d, uint32_t a)
{
    e.h(0x9F7A0, a, unit, d, 0, 0, a);
    const uint32_t v = e.h(0xA460, 0, 0, 0, 0, 0, unit);
    return e.h(0xA472, 0, 0, 0, 0, 0, unit, 0xa, v, 0);
}
}
