// 1.13c D2Client+0x18650: EAX = name; table index via 0xC42A(name, srcfile, 0x75A), row (stride 0x23C) byte +5 tested against bit 0 of gdwBitMasks[0] -> 1/0
#include "cl17.h"
extern "C" uint32_t d2c_18650(uint32_t name, const cl::Env& e) {
    if (!name) return 0;
    int32_t idx = (int32_t)e.H(0xc42a, name, 0xB2000000u | 0xD6F7Cu, 0x75a);
    uint32_t row = 0;
    if (idx >= 0 && idx < (int32_t)e.R(e.dt + 0xba0)) row = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    uint32_t dl = e.R8(row + 5);          // row 0 faults on retail; never vectored
    return (e.R8(e.bm) & dl) != 0;
}
