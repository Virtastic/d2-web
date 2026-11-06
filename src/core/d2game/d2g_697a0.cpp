// 1.13c D2Game+0x697A0 -- DrainManaIntoLife. ECX = effect context (+8 unit), EDX = record (+4, +8 percentages).
// Written from the 1.13c disassembly: amt = (pct1 * stat8) / 100 (32-bit product, unsigned divide); stat 8 -= amt through
// 0xA4D2; stat 6 += (pct2 * amt) / 100. Stat reads are 0xA42A(unit, stat, 0), writes 0xA4D2(unit, stat, delta, 0).
#include "../fn/hookenv.h"
namespace d2g {
static uint32_t div100(uint32_t x) { return (uint32_t)(((uint64_t)x * 0x51eb851fu) >> 32) >> 5; }   // retail magic multiply: not exact above 2^31
uint32_t f697a0(HookEnv& e, uint32_t ecx, uint32_t edx)
{
    const uint32_t u = e.rd32(ecx + 8);
    const uint32_t cur = e.h(0xA42A, 0, 0, 0, 0, 0, u, 8, 0);
    const uint32_t amt = div100(e.rd32(edx + 4) * cur);
    e.h(0xA4D2, 0, 0, 0, 0, 0, u, 8, 0u - amt, 0);
    return e.h(0xA4D2, 0, 0, 0, 0, 0, u, 6, div100(e.rd32(edx + 8) * amt), 0);
}
}
