// 1.13c D2Multi+0x11510: when DAT_6fa09dac != 0, destroys its control set (5 or 6 slots by mode 1/other, then 7 more),
// clears the mode, then Ordinal_10031(DAT_6fa09160) and Ordinal_10053(DAT_6fa0a0d0) when set. Returns the mode word to store (always 0 once run).
#include "d2multi_env.h"

uint32_t d2multi_11510(const D2mEnv& e, uint32_t mode, uint32_t dat9160, uint32_t dat0d0) {
    if (mode) {
        static const uint16_t m1[5] = { 0x9d64, 0x9d68, 0x9d6c, 0x9d70, 0x9d74 };
        static const uint16_t m2[6] = { 0x9d78, 0x9d7c, 0x9d80, 0x9d84, 0x9d88, 0x9d8c };
        static const uint16_t tail[7] = { 0x9d90, 0x9d94, 0x9d98, 0x9d9c, 0x9da0, 0x9da4, 0x9da8 };
        if (mode == 1) for (uint16_t s : m1) d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x30000 + s));
        else for (uint16_t s : m2) d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x30000 + s));
        mode = 0;
        for (uint16_t s : tail) d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x30000 + s));
    }
    d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, dat9160);
    if (dat0d0) d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, dat0d0);
    return mode;
}
