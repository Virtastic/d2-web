// 1.13c D2Game+0x69800 -- DrainLifeIntoMana. ECX = effect context (+8 unit), EDX = record. Written from the 1.13c disassembly.
// Life (stat 6) is 8.8 fixed point: amt = (pct1 * (stat6 >> 8, arithmetic)) / 100 unsigned; stat 6 -= amt << 8;
// stat 8 += (pct2 * (amt << 8)) / 100.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f69800(HookEnv& e, uint32_t ecx, uint32_t edx)
{
    const uint32_t u = e.rd32(ecx + 8);
    const int32_t cur = (int32_t)e.h(0xA42A, 0, 0, 0, 0, 0, u, 6, 0);
    const uint32_t amt = (e.rd32(edx + 4) * (uint32_t)(cur >> 8)) / 100u;
    const uint32_t amt256 = amt << 8;
    e.h(0xA4D2, 0, 0, 0, 0, 0, u, 6, 0u - amt256, 0);
    return e.h(0xA4D2, 0, 0, 0, 0, 0, u, 8, (e.rd32(edx + 8) * amt256) / 100u, 0);
}
}
