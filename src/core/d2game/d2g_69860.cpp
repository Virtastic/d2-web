// 1.13c D2Game+0x69860 -- bring stat 8 up (or down) to 0xA4EA(unit). ECX = effect context (+8 unit). Written from the 1.13c disassembly.
// d = 0xA4EA(unit) - 0xA42A(unit, 8, 0); a non-zero d is added through 0xA4D2(unit, 8, d, 0). The result is that call's EAX, else 0.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f69860(HookEnv& e, uint32_t ecx)
{
    const uint32_t u = e.rd32(ecx + 8);
    const uint32_t cur = e.h(0xA42A, 0, 0, 0, 0, 0, u, 8, 0);
    const uint32_t d = e.h(0xA4EA, 0, 0, 0, 0, 0, u) - cur;
    if (!d) return 0;
    return e.h(0xA4D2, 0, 0, 0, 0, 0, u, 8, d, 0);
}
}
