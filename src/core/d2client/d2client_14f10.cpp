// 1.13c D2Client+0x14F10: ECX = unit, one ignored stack word (ret 4). Clears the first parameter of the unit's used skill; 0 without one.
#include "d2c_seam.h"
extern "C" uint32_t d2c_14f10(const D2cSeam* s, const uint8_t* unit) {
    uint64_t used = (*s)(0xc310, 0, 0, 0, 0, 0, (uintptr_t)unit);
    if (!(uint32_t)used) return 0;
    (*s)(0xcb50, 0, 0, 0, 0, 0, used, 0);
    return 1;
}
