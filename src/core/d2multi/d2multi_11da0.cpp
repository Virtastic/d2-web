// 1.13c D2Multi+0x11DA0: destroys eleven controls (Ordinal_10050 on each control slot), then Ordinal_10031(DAT_6fa09160).
#include "d2multi_env.h"

void d2multi_11da0(const D2mEnv& e, uint32_t dat9160) {
    static const uint16_t slot[11] = { 0x9cc8, 0x9cd4, 0x9cd8, 0x9cc0, 0x9cc4, 0x9ccc, 0x9cd0, 0x9cdc, 0x9ce0, 0x9ce4, 0x9ce8 };
    for (uint16_t s : slot) d2m_call(e, 0x7c8c, 0, 0, 0, 0, 0, D2M_MOD(0x30000 + s));
    d2m_call(e, 0x7c38, 0, 0, 0, 0, 0, dat9160);
}
