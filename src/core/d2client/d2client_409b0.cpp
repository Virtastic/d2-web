// 1.13c D2Client+0x409B0: ECX = unit, one ignored stack word (ret 4).
#include "d2c_seam.h"
extern "C" uint32_t d2c_409b0(const D2cSeam* s, const uint8_t* unit) {
    uint64_t res = d2cUnitRes(*s, unit);
    if (!res) return 0;
    uint64_t a = (*s)(0xc1f0, 0, 0, 0, 0, 0, (uintptr_t)unit);
    if ((*s)(0xc208, 0, 0, 0, 0, 0, a)) return 0;
    uint64_t b = (*s)(0xc1f0, 0, 0, 0, 0, 0, res);
    if ((*s)(0xc208, 0, 0, 0, 0, 0, b)) return 0;
    uint32_t t; std::memcpy(&t, (const void*)(uintptr_t)res, 4);
    if (t > 1) return 0;
    return (*s)(0xa62a0, res, 0, 0, 0, 0, (uintptr_t)unit) != 0;
}
