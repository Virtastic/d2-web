// 1.13c D2Client+0x18F40: EAX = index into the word table at D2Client+0xF8508 (count at +0xF852C); a non-zero entry goes to the 0x23200 import (s0, entry) (ret 4)
// g = host address of the module image bytes at RVA 0xF8508 (table words at g + 4*i, count at g + 0x24); the out-of-range branch logs and exits, never vectored
#include "cl17.h"
extern "C" uint32_t d2c_18f40(uint32_t idx, uint32_t s0, uint32_t g, const cl::Env& e) {
    if (!((int32_t)e.R(g + 0x24) > (int32_t)idx)) return 0;
    uint32_t v = e.R(g + idx * 4) & 0xffff;
    if (!v) return v;
    return e.H(0x23200, s0, v);
}
