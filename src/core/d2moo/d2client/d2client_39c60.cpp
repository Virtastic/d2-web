// 1.13c D2Client+0x39C60 -- key command: flip the flag at 0x11C3EC between 0 and 1; the new value is left in EAX.
#include "cl_cmd.h"
extern "C" uint32_t d2c_39c60(uint32_t, const cl::Env& e) {
    const uint32_t v = clc::G(e, 0x11C3EC) == 0;
    clc::S(e, 0x11C3EC, v);
    return v;
}
