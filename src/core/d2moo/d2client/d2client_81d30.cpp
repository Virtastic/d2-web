// Mode 2 only: forward the value to the common setter, then set or clear bit 1 of the unit flag word at +0xC4.
#include "cl_env.h"
// 1.13c D2Client+0x81D30. EAX = mode, ESI = unit, stack: on (bit 1 set when nonzero), value.
extern "C" void d2cl_81d30(uint32_t mode, uint32_t unit, uint32_t on, uint32_t val, const cl::Env& e) {
    if (mode != 2) return;
    e.H(0xc43c, unit, val);
    if (!unit) return;
    uint32_t f = e.R(unit + 0xc4);
    e.W(unit + 0xc4, on ? (f | 2u) : (f & ~2u));
}
