// 1.13c D2Client+0xAC4F0: no arguments. 0xD252(1, 0, &byte) with the byte 0x6B (stdcall 3), then counts up the module dwords at D2Client+0x11C4E0 and
// +0x11C4E8. EAX is the new value of the second.
#include "d2c_seam.h"
extern "C" uint32_t d2c_ac4f0(const D2cSeam* s, uint32_t* g1, uint32_t* g2) {
    uint32_t loc = 0x6b;   // only the low byte is retail's; the rest is stack residue (masked by the capture)
    (*s)(0xd252, 0, 0, 0, 0, 0, 1, 0, (uint64_t)(uintptr_t)&loc);
    ++*g1;
    return ++*g2;
}
