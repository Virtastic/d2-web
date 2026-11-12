// 1.13c D2Client+0x39E20 -- key command: step the mode word at 0x11C208 to the next of four and hand it on in EAX.
// With 0x11C1B0 == 1 and the mode at 1, the mode first becomes 2 when 0x11C20C is clear, else 0. The step is a signed
// remainder by four of mode + 1.
#include "cl_cmd.h"
extern "C" uint32_t d2c_39e20(uint32_t, const cl::Env& e) {
    if (clc::G(e, 0x11C1B0) == 1 && clc::G(e, 0x11C208) == 1) clc::S(e, 0x11C208, clc::G(e, 0x11C20C) ? 0u : 2u);
    const int32_t next = (int32_t)(clc::G(e, 0x11C208) + 1u) % 4;
    return e.H(0x5f970, 0, 0, 0, 0, 0, (uint32_t)next);
}
