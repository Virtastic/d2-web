// 1.13c D2Client+0x72BA0: ECX = unit. Forwards to 0x20A60 with ESI = unit and returns what it returns.
#include "d2c_seam.h"
extern "C" uint32_t d2c_72ba0(const D2cSeam* s, const uint8_t* unit) {
    return (uint32_t)(*s)(0x20a60, 0, 0, 0, 0, (uintptr_t)unit);
}
