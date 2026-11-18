// 1.13c D2Client+0x40670: ECX = unit, one ignored stack word (ret 4). Pops one owner list entry when the owner has more than one.
#include "d2c_seam.h"
extern "C" uint32_t d2c_40670(const D2cSeam* s, uint8_t* unit) {
    uint64_t o = (*s)(0xc310, 0, 0, 0, 0, 0, (uintptr_t)unit);
    if (!o) return 0;
    if (!d2cUnitRes(*s, unit)) return 0;
    int32_t n = (int32_t)(*s)(0xcb68, 0, 0, 0, 0, 0, o) - 1;
    if (n > 0) { (*s)(0xcb50, 0, 0, 0, 0, 0, o, (uint64_t)n); uint32_t z = 0; std::memcpy(unit + 0x44, &z, 4); }
    return 1;
}
