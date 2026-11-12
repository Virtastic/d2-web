// 1.13c D2Client+0x39D80 -- key command: flip the config flag at 0x11C240, store it under its key name, and copy the
// new value to the two words after it (0x11C244, 0x11C248) before the flag itself.
#include "cl_cmd.h"
extern "C" uint32_t d2c_39d80(uint32_t, const cl::Env& e) {
    uint32_t v;
    const uint32_t r = clc::cfgFlip(e, 0x11C240, 0x6FB853C4u, v);
    clc::S(e, 0x11C244, v);
    clc::S(e, 0x11C248, v);
    clc::S(e, 0x11C240, v);
    return r;
}
