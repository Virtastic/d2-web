// 1.13c D2Client+0xB2570 (ecx = unit, edx = target): returns 1; when the target is non-zero and the unit's class row has a positive short at +0x36
// it first calls D2Client+0x6B300(target, short, 2, 0, 0, 0, 0).
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xB2570
extern "C" uint32_t d2c_b2570(uint32_t u, uint32_t t, const cl::Env& e) {
    if (!t) return 1;
    uint32_t row = e.UnitRow(u);
    if (row && e.R16s(row + 0x36) > 0) e.H(0x6b300, t, 2, 0, 0, 0, (uint32_t)e.R16s(row + 0x36));   // seven stack words: target, 2, five zeros; EAX = the short
    return 1;
}
