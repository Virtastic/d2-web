// 1.13c D2Game+0x4C190 -- ECX = unit, EDX = amount. A zero amount returns with EAX untouched. Otherwise the D2Common import 0xA42A(unit, 0x19, 0) reads the stat and 0xA472(unit, 0x19, stat + amount, 0) stores the sum; its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f4c190(GE& e, uint32_t eax, uint32_t unit, uint32_t amount)
{
    if (!amount) return eax;
    const uint32_t v = e.h(0xA42A, 0, 0, 0, 0, 0, unit, 0x19, 0);
    return e.h(0xA472, 0, 0, 0, 0, 0, unit, 0x19, v + amount, 0);
}
}
