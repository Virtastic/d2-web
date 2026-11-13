// 1.13c D2Client+0x72280 -- state-on callback (ECX = unit): sets bit 0x40000 of the unit's +0xC8 word. EAX is
// untouched.
#include "cl_pa4.h"
extern "C" uint32_t d2c_72280(uint32_t eax, uint32_t ecx, const cl::Env& e) {
    if (ecx) e.W(ecx + 0xc8, e.R(ecx + 0xc8) | 0x40000u);
    return eax;
}
