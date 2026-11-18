// 1.13c D2Client+0x40400 -- skill start function (ECX = unit, EDX = skill id, one stack word): plays the skill's
// cast overlay (the row dword at +0x154, 0 for an id outside the table) on the unit (EDI). Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_40400(uint32_t ecx, uint32_t edx, const cl::Env& e) {
    const uint32_t row = pa4::skRow(e, edx);
    e.H(0xa0880, row ? e.R(row + 0x154) : 0);
    e.Ej(0xa0880, ecx);
    return 1;
}
