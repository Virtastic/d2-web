// 1.13c D2Client+0x72240 -- state-off callback (ECX = unit): clears bit 0x40000 of the unit's +0xC8 word and bit
// 0x20 of its +0xC4 word. EAX is untouched.
#include "cl_pa4.h"
extern "C" uint32_t d2c_72240(uint32_t eax, uint32_t ecx, const cl::Env& e) {
    if (ecx) {
        e.W(ecx + 0xc8, e.R(ecx + 0xc8) & 0xfffbffffu);
        e.W(ecx + 0xc4, e.R(ecx + 0xc4) & 0xffffffdfu);
    }
    return eax;
}
