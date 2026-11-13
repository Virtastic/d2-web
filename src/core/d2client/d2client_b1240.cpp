// 1.13c D2Client+0xB1240 (ecx = unit, edx = request block): for a type 1 unit, 1 when D2Client+0xB1000 (as in 0xB11D0) returns non-zero; otherwise 0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB1240
extern "C" uint32_t d2c_b1240(uint32_t u, uint32_t b, const cl::Env& e) {
    if (!u || e.R(u) != 1) return 0;
    uint32_t w0 = e.R(b);
    uint32_t r = e.H(0xb1000, w0, 0, 0, 0, 0, 0, u, b, e.R(b + 8), u);
    e.Edj(0xb1000, e.R(b + 4));
    return r != 0;
}
