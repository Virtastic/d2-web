// 1.13c D2Multi+0xBDD0: string 0x147b (D2Lang lookup) goes to the window-text rule AA60, the state word DAT_6fa0a0dc := 4, then the
// D2Win #10199 refresh. Stdcall ret 4, the argument is unused; returns 1. g: [3a0dc] (out).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_bdd0(const D2mEnv& e, uint32_t* g) {
    const uint32_t s = d2m_call(e, 0x7ea8, 0, 0x147b);
    d2m_call(e, 0xaa60, 0, 0, 0, 0, 0, s);
    g[0] = 4;
    d2m_call(e, 0x7ca4);
    return 1;
}
