// 1.13c D2Client+0x436E0 -- the five-step teardown: five argument-less callees (0x8A540, 0x16620, 0xAB660, 0x9D500, 0x21630),
// then Ordinal 10048 (no args) whose result is the single stack argument of Ordinal 418 (via thunk 0xBF56). Returns 1.
#include "cl_env.h"
extern "C" uint32_t d2c_436e0(const cl::Env& e) {
    e.H(0x8a540);
    e.H(0x16620);
    e.H(0xab660);
    e.H(0x9d500);
    e.H(0x21630);
    uint32_t v = e.H(0xd0b4);
    e.H(0xbf56, v);
    return 1;
}
