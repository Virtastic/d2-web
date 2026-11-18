// 1.13c D2Client+0x64730 -- options item (automap mode): load the saved mode (clamped to 0..2), apply it when it differs from the live one, mirror the live mode into the value slot.
#include "cl_cmd.h"
extern "C" uint32_t d2c_64730(uint32_t it, const cl::Env& e) {
    uint32_t loc = 0;
    const uint32_t la = (uint32_t)(uintptr_t)&loc;
    e.H(0xbe8a, 0x6FB83588u, 0x6FB851B0u, 0, la);
    uint32_t eax = e.R(la);
    if (eax > 2) eax = 0;
    uint32_t ecx = clc::G(e, 0x11C1B0);
    if (eax != ecx) {
        clc::S(e, 0x11C1B0, eax);
        clc::S(e, 0xF16B0, eax == 1 ? 0x14 : 0xA);
        e.H(0x5fb50);
        eax = e.H(0x608d0, 0, 0, 0, 0, 0, 0, 1);
        ecx = clc::G(e, 0x11C1B0);
    }
    e.W(it + 0x124, ecx);
    return eax;
}
