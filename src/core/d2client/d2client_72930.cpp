// 1.13c D2Client+0x72930 -- clear flag 0x10 in the first dword of the unit's block at (unit+0x54)->+0x30 and zero that block's
// dwords 1..3. ECX = unit. Returns the EAX left: the block pointer, or 0.
#include "cl_env.h"
extern "C" uint32_t d2c_72930(uint32_t unit, const cl::Env& e) {
    if (!unit) return 0;
    const uint32_t d = e.R(unit + 0x54);
    if (!d) return 0;
    const uint32_t p = e.R(d + 0x30);
    if (!p) return 0;
    e.W(p, e.R(p) & 0xffffffefu);
    e.W(p + 4, 0); e.W(p + 8, 0); e.W(p + 0xc, 0);
    return p;
}
