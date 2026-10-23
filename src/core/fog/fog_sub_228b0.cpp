// 1.13c Fog+0x228B0: MSVC CRT _CIpow (x87 args: ST1 = base, ST0 = exponent, result in ST0).
// Retail picks an SSE2 pow (Fog+0x23400) when the SSE2 flag is set and MXCSR/FPU control words are
// the default all-masked state, else the x87 fallback (Fog+0x2290D). Both compute pow(base, exp).
#include <cmath>
extern "C" double fog_sub_228b0(double base, double exp) {  // 1.13c Fog+0x228B0
    return std::pow(base, exp);
}
extern "C" double fog_sub_23400(double base, double exp) {  // 1.13c Fog+0x23400
    return fog_sub_228b0(base, exp);
}
