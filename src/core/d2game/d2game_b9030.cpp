// 1.13c D2Game+0xB9030: 0xAB02(0x6D, &local) gates; then 0xAC1C(record(0xA4C0(handle))[0x1C + unit[0x6D] * 4], local).
#include <cstdint>
struct GB9030Env { uint32_t (*fab02)(uint32_t s0, uint32_t* local); uint32_t (*fa4c0)(uint32_t s0); void (*fac1c)(uint32_t s0, uint32_t s1); };
void g_b9030(uint32_t unit, uint32_t handle, const GB9030Env& env) {
    uint32_t local = 0;
    if (env.fab02(0x6D, &local) != 0) {
        uint32_t pd = env.fa4c0(handle);
        uint8_t slot = *(const uint8_t*)(uintptr_t)(unit + 0x6D);
        env.fac1c(*(const uint32_t*)(uintptr_t)(pd + 0x1C + (uint32_t)slot * 4), local);
    }
}
