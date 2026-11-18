// 1.13c Fog+0x228B0: MSVC CRT _CIpow (x87 args: ST1 = base, ST0 = exponent, result in ST0).
// Retail picks the SSE2 path (thunk Fog+0x23400 -> body Fog+0x23419) when the SSE2 flag DAT_6ffa712c
// (Fog+0x5712C) is set and MXCSR/FPU exception masks are the default all-masked state (always so on this
// host), else the x87 fallback (Fog+0x2290D). Both compute pow(base, exp). Same bytes as Storm+0x3C9C0.
#include <cstdint>
#include "core/crt/msvc_pow.h"
namespace {
double fog_pow_default(double b, double e) { return d2::crt::msvc_pow(b, e); }
}
extern "C" { uint32_t fog_sse2Flag_5712c = 1; }  // Fog+0x5712C: set by the CRT start-up on SSE2 hosts

// 1.13c Fog+0x23400: SSE2 thunk; spills x (ST1) and y (ST0) as stack doubles and calls the body.
double fog_23400_thunk(double base, double exp, double (*body)(double, double)) {  // 1.13c Fog+0x23400
    return body(base, exp);
}
double fog_228b0_dispatch(double base, double exp, double (*sse2Body)(double, double),
                          double (*x87Body)(double, double)) {  // 1.13c Fog+0x228B0
    if (fog_sse2Flag_5712c != 0) return fog_23400_thunk(base, exp, sse2Body);
    return x87Body(base, exp);
}
extern "C" double fog_sub_228b0(double base, double exp) {  // 1.13c Fog+0x228B0
    return fog_228b0_dispatch(base, exp, fog_pow_default, fog_pow_default);
}
extern "C" double fog_sub_23400(double base, double exp) {  // 1.13c Fog+0x23400
    return fog_23400_thunk(base, exp, fog_pow_default);
}
