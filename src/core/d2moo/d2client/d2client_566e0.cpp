// 1.13c D2Client+0x566E0: ECX = unit, one ignored stack word (ret 4). Clears the flags of the unit's used skill; 0 without one.
#include "d2c_seam.h"
extern "C" uint32_t d2c_566e0(const D2cSeam* s, const uint8_t* unit) {
    uint64_t used = (*s)(0xc310, 0, 0, 0, 0, 0, (uintptr_t)unit);
    if (!(uint32_t)used) return 0;
    (*s)(0xc60a, 0, 0, 0, 0, 0, used, 0);
    return 1;
}
