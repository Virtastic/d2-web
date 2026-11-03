// MSVC (VS2003) CRT pow as Fog's _CIpow (1.13c Fog+0x228B0) reaches it on an SSE2 host: both x87
// operands are stored as doubles (Fog+0x23400 `fstp qword` x2) and the SSE2 body returns a double.
#pragma once

namespace d2::crt {
double msvc_pow(double base, double exp);
}  // namespace d2::crt
