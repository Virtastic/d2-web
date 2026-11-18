// 1.13c D2Client+0x64370 -- true when the flag at 0x11C280 is set and the renderer mode query answers 1 or 2.
#include "cl_cmd.h"
extern "C" uint32_t d2c_64370(uint32_t, const cl::Env& e) {
    if (!clc::G(e, 0x11C280)) return 0;
    const uint32_t r = e.H(0xd324);
    return r == 1 || r == 2;
}
