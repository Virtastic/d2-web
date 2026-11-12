// 1.13c D2Client+0x14F30: ECX = unit, one ignored stack word (ret 4). The truth value of the unit gate at 0xA0A30 (EAX = unit).
#include "d2c_seam.h"
extern "C" uint32_t d2c_14f30(const D2cSeam* s, const uint8_t* unit) {
    return (uint32_t)(*s)(0xa0a30, (uintptr_t)unit) != 0;
}
