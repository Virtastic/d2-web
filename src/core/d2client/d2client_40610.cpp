// 1.13c D2Client+0x40610: ECX = unit, one ignored stack word (ret 4). True when the unit's record type is 0 or 1.
#include "d2c_seam.h"
extern "C" uint32_t d2c_40610(const D2cSeam* s, const uint8_t* unit) {
    uint64_t r = d2cUnitRes(*s, unit);
    if (!r) return 0;
    uint32_t t; std::memcpy(&t, (const void*)(uintptr_t)r, 4);
    return t == 0 || t == 1;
}
