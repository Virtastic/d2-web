// 1.13c D2Multi+0xC720: D2Win #10097 (7C1A, EDX = 0) on the control slots [39158] [3915c] [39150], then BE00 and B970. Stdcall ret 4,
// the argument is unused; returns 1. g: +0 [39150], +4 [39154], +8 [39158], +12 [3915c].
#include "d2multi_env.h"

extern "C" uint32_t d2multi_c720(const D2mEnv& e, const uint32_t* g) {
    d2m_call(e, 0x7c1a, 0, g[2], 0);
    d2m_call(e, 0x7c1a, 0, g[3], 0);
    d2m_call(e, 0x7c1a, 0, g[0], 0);
    d2m_call(e, 0xbe00);
    d2m_call(e, 0xb970);
    return 1;
}
