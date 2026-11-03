// 1.13c D2Multi+0x11A20: destroys thirty controls (Ordinal_10050 on each control slot), Ordinal_10031(DAT_6fa09160), then Ordinal_10053(DAT_6fa0a0d0) when set.
#include "d2multi_env.h"

void d2multi_11a20(const D2mEnv& e, uint32_t dat9160, uint32_t dat0d0) {
    static const uint16_t slot[30] = { 0x9cec, 0x9cf0, 0x9cf4, 0x9cf8, 0x9cfc, 0x9d00, 0x9d04, 0x9d08, 0x9d2c, 0x9d30, 0x9d34, 0x9d38,
        0x9d3c, 0x9d28, 0x9d48, 0x9d40, 0x9d4c, 0x9d50, 0x9d54, 0x9d58, 0x9d5c, 0x9d60, 0x9d44, 0x9d14, 0x9d18, 0x9d1c, 0x9d20,
        0x9d24, 0x9d0c, 0x9d10 };
    for (uint16_t s : slot) d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x30000 + s));
    d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, dat9160);
    if (dat0d0) d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, dat0d0);
}
