// 1.13c D2Multi+0xC6E0: D2Win #10097 (7C1A, EDX = 0) on the control slots [39154] [39158] [39150], then BE00 and A900. Stdcall ret 4,
// the argument is unused; returns 1. g: +0 [39150], +4 [39154], +8 [39158].
#include "d2multi_env.h"

extern "C" uint32_t d2multi_c6e0(const D2mEnv& e, const uint32_t* g) {
    d2m_call(e, 0x7c1a, 0, g[1], 0);
    d2m_call(e, 0x7c1a, 0, g[2], 0);
    d2m_call(e, 0x7c1a, 0, g[0], 0);
    d2m_call(e, 0xbe00);
    d2m_call(e, 0xa900);
    return 1;
}
