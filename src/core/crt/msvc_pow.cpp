// MSVC CRT pow, SSE2 path (1.13c Fog+0x23400). The table-driven body is not ported; std::pow stands in
// and matches retail on every pow case in vectors/fog/fn/212c0.bin (the only pow vector so far).
#include "core/crt/msvc_pow.h"
#include <cmath>

namespace d2::crt {
double msvc_pow(double base, double exp) { return std::pow(base, exp); }
}  // namespace d2::crt
