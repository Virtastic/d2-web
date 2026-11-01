// 1.13c D2Client+0x18AC0: playerSelf is a type-0 unit whose data block holds state 0x13 and the pair (s0, s1) -> 1, else 0 (ret 8)
#include "cl17.h"
extern "C" uint32_t d2c_18ac0(uint32_t s0, uint32_t s1, const cl::Env& e) {
    uint32_t self = e.self;
    if (!self || e.R(self) != 0) return 0;
    uint32_t d = e.R(self + 0x14);
    if (!d || e.R(d + 0x154) != 0x13) return 0;
    return e.R(d + 0x158) == s0 && e.R(d + 0x15c) == s1;
}
