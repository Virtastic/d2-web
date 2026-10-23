// 1.13c Fog+0x2290D: MSVC CRT x87 pow fallback (ST1 = base, ST0 = exponent, result in ST0).
// Reached from _CIpow (Fog+0x228B0) when the SSE2 path (Fog+0x23400) is not eligible; same pow(base, exp).
#include <cmath>
extern "C" double fog_sub_2290d(double base, double exp) {  // 1.13c Fog+0x2290D
    return std::pow(base, exp);
}
