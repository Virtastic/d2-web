// 1.13c D2Client+0xA4410 -- stdcall(arg): p = [0x11BC00]. When [p+0x20] != 0 or [0xFAD94] == 0, FUN 0xA3BB0([p+0x1C] == 0) and its
// return is the function's. Otherwise EAX still holds p.
#include "cl_cmd.h"
extern "C" uint32_t d2c_a4410(uint32_t, const cl::Env& e) {
    uint32_t p = clc::G(e, 0x11BC00);
    if (e.R(p + 0x20) != 0 || clc::G(e, 0xFAD94) == 0) return e.H(0xa3bb0, e.R(p + 0x1c) == 0 ? 1u : 0u);
    return p;
}
