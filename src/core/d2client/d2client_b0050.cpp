// 1.13c D2Client+0xB0050 (eax = v, passed on as the stack argument of thunk 0xC4D2 = Ordinal 10985): bucket of its result
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB0050
extern "C" int32_t d2c_b0050(uint32_t eax, const cl::Env& e) {
    int32_t v = (int32_t)e.H(0xc4d2, eax);
    if (v < 0x7d) return 0x19;
    if (v < 0xaf) return 10;
    return ((v >= 0x12c ? 1 : 0) - 1 & -12) + 0xf;
}
