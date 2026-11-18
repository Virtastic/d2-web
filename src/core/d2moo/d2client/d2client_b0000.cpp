// 1.13c D2Client+0xB0000 (eax = v, ecx = p): level-based step count, clamped
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0000
extern "C" int32_t d2c_b0000(int32_t v, uint32_t p, const cl::Env& e) {
    if (v < 0x1e) return 1;
    if (v < 0x55) { int32_t d = v - 15; return (d + ((d >> 31) & 3)) >> 2; }
    int32_t s = e.R16s(p + 0x96), t = s - 0x32;
    if (v < t) return 12;
    if (v < s) { int32_t d = v - t; return 12 - ((d + ((d >> 31) & 3)) >> 2); }
    return 1;
}
