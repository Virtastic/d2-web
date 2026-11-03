// 1.13c D2Multi+0xED60: like 0xEC80 but appends: dst := dst + token + decimal(v) (an empty dst just receives token + decimal; fatal at 500 characters or more).
#include "d2multi_env.h"
#include "d2multi_ucs.h"
#include "d2multi_numtext.h"

void d2multi_ed60(const D2mEnv& e, int32_t v, uint16_t* dst) {
    uint16_t buf[512], num[8];
    d2m_ucpy(buf, (const uint16_t*)(uintptr_t)d2m_call(e, 0x7ea8, 0, 0xf9a));
    d2m_numtext(num, v);
    d2m_ucat(buf, num);
    if (!*dst) { d2m_ucpy(dst, buf); return; }
    if (d2m_ulen(dst) >= 500) {
        uint32_t m = d2m_call(e, 0x7bde, 0, 0, 0, 0, 0, 0xb60);
        d2m_call(e, 0x7bd2, 0, 0, 0, 0, 0, D2M_MOD(0x175d7), m);
        d2m_call(e, 0x14ba, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    d2m_ucat(dst, buf);
}
