// 1.13c D2Multi+0xC690: vtable slot +0x1c (0x7c0e stands for it in the capture), BE00, rebuild the four control slots, [39f20] := 0.
// gv: +0 [39154], +4 [39150], +8 [39158], +12 [3915c], +16 [39f20] (out).
#include "d2multi_env.h"

extern "C" void d2multi_c690(const D2mEnv& e, uint32_t* gv) {
    d2m_call(e, 0x7c0e, 0, 0, 0);
    d2m_call(e, 0xbe00);
    d2m_call(e, 0x7c1a, 0, gv[0], 0);
    d2m_call(e, 0x7c1a, 0, gv[1], 0);
    d2m_call(e, 0x7c1a, 0, gv[2], 0);
    d2m_call(e, 0x7c1a, 0, gv[3], 0);
    gv[4] = 0;
}
