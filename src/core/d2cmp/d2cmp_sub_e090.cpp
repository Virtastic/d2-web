// D2CMP cel-context direction-scale setter; callees injected (12190, 17020, fatal exit).
// ponytail: decompile leaves the divisor as extraout_ECX; taken as param_2 (callers pass it in ECX).
#include <cstdint>

// 1.13c D2CMP+0x0E090
void d2cmp_e090(uint8_t* self, int p1, uint32_t p2, uint8_t* p3, int dat87f00,
                uint32_t (*d12190)(int, int), int (*d17020)(), void (*fatal)())
{
    if (!p3 || !self) return;
    uint32_t m = *(uint32_t*)(self + 8), f = self[4], k = *(uint32_t*)(self + 0x10);
    bool b = true;
    if ((m != 6 || !(f & 2)) && m == 0 && (!(f & 8) || !k || k == 0x11 || dat87f00 != 0)) b = false;
    if (p1 == 0 && p2 > 1 && b) {
        *(uint32_t*)(self + 0x40) = *(uint32_t*)(p3 + 0x14) < 2 ? 1 : (d12190(p2, p2 * 2) << 6) / p2;
        if (!d17020()) fatal();
    }
}
