// 1.13c D2Multi+0x11E20: EAX = row index into the 0x3b-byte table at module+0x1bf20 (name text at row+0x11). When the control DAT_6fa09cc4 exists:
// 0x7cb0, two 0x7cbc text appends when the row text is non-empty, 0x7cd4, then 0x7c44 on DAT_6fa09cd4 and Ordinal_10053(0).
#include "d2multi_env.h"
#include <cstring>

void d2multi_11e20(const D2mEnv& e, uint32_t idx, uint32_t ctl, uint32_t ctl2, const uint8_t* row) {
    if (!ctl) return;
    const uint32_t rowRva = 0x1bf20 + idx * 0x3b;
    d2m_call(e, 0x7cb0, 0, ctl);
    if (strlen((const char*)row + 0x11) != 0) {
        d2m_call(e, 0x7cbc, 0, ctl, D2M_MOD(rowRva + 0x11));
        d2m_call(e, 0x7cbc, 0, ctl, D2M_MOD(0x175d7));
    }
    d2m_call(e, 0x7cd4, 0, ctl, 0);
    d2m_call(e, 0x7c44, 0, ctl2);
    d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, 0);
}
