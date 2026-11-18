// 1.13c D2Client+0xB11D0 (ecx = unit, edx = request block): for a unit of type 0 or 1, 1 when D2Client+0xB1000 (stack word = block dword 0, ebx = dword 2,
// edi = dword 1, esi = unit) returns non-zero; otherwise 0.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB11D0
extern "C" uint32_t d2c_b11d0(uint32_t u, uint32_t b, const cl::Env& e) {
    if (!u) return 0;
    uint32_t ty = e.R(u);
    if (ty != 0 && ty != 1) return 0;
    uint32_t w0 = e.R(b);
    uint32_t r = e.H(0xb1000, w0, 0, 0, 0, 0, 0, u, b, e.R(b + 8), u);
    e.Edj(0xb1000, e.R(b + 4));
    return r != 0;
}
