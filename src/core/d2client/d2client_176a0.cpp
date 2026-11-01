// 1.13c D2Client+0x176A0: walk the unit (EDI) at the target unit (EAX, may be 0): stack (s0 = action ecx, s1 = block word 4), ret 8
#include "cl17.h"
extern "C" uint32_t d2c_176a0(uint32_t tgt, uint32_t unit, uint32_t s0, uint32_t s1, const cl::Env& e) {
    uint32_t path = e.R(unit + 0x2c);
    uint32_t type = tgt ? e.R(tgt) : 6, id = tgt ? e.R(tgt + 0xc) : 0xffffffffu;
    return cl17::walk(e, unit, path, type, id, s1, s0, unit);
}
