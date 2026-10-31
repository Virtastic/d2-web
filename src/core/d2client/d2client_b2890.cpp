// 1.13c D2Client+0xB2890 (ecx = unit, void): look the unit's A60D0 partner up; none -> B1720. A partner whose first
// word is 0 or 1 and accepted by D2Client+0xA5440 re-enters B1420(unit, 0); otherwise run ordinals 11121 / 10759.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2890
extern "C" void d2c_b2890(uint32_t u, const cl::Env& e) {
    uint32_t p = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!p) { e.H(0xb1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, u); return; }
    int32_t k = (int32_t)e.R(p);
    if (k >= 0 && k <= 1 && e.H(0xa5440, 0, 0, 0, 0, 0, p)) { e.H(0xb1420, 0, 0, 0, 0, 0, 0, u); return; }
    e.H(0xc346, u, p);
    e.H(0xc3a6, u);
}
