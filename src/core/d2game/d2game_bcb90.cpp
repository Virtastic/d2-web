// 1.13c D2Game+0xBCB90: reset record (+0x64 = -1, +0x68 = 6, byte +0x6C = 0), then 0x48300(ESI, arg) with
// ESI = unit[0x14][0x30] for a type-1 unit with a +0x14 pointer, else 0.
#include <cstdint>
struct GBcb90Env { uint32_t (*f48300)(uint32_t esi, uint32_t s0); };
uint32_t g_bcb90(uint32_t unit, uint32_t rec, uint32_t arg, const GBcb90Env& env) {
    uint8_t* r = (uint8_t*)(uintptr_t)rec;
    *(int32_t*)(r + 0x64) = -1;
    *(int32_t*)(r + 0x68) = 6;
    r[0x6C] = 0;
    uint32_t esi = 0;
    if (unit) {
        const uint32_t* u = (const uint32_t*)(uintptr_t)unit;
        if (u[0] == 1 && u[5]) esi = *(const uint32_t*)(uintptr_t)(u[5] + 0x30);
    }
    return env.f48300(esi, arg);
}
