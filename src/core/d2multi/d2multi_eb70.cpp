// 1.13c D2Multi+0xEB70: stdcall (Unicode* s). Strings of 500 or more characters are fatal. Fetches GetStringByIndex(0xF9A) (a colour-code token) and
// deletes every occurrence of it, together with the one character that follows it, while that following character is not the terminator.
#include "d2multi_env.h"
#include "d2multi_ucs.h"

void d2multi_eb70(const D2mEnv& e, uint16_t* s) {
    if (d2m_ulen(s) >= 500) {
        uint32_t m = d2m_call(e, 0x7bde, 0, 0, 0, 0, 0, 0x2ef);
        d2m_call(e, 0x7bd2, 0, 0, 0, 0, 0, D2M_MOD(0x175d7), m);
        d2m_call(e, 0x14ba, 0, 0, 0, 0, 0, 0xffffffffu);
        return;
    }
    const uint16_t* tok = (const uint16_t*)(uintptr_t)d2m_call(e, 0x7ea8, 0, 0xf9a);
    int32_t tl = d2m_ulen(tok);
    uint16_t* p = d2m_ustr(s, tok);
    while (p && p[tl] != 0) {
        int32_t n = d2m_ulen(p);
        memmove(p, p + tl + 1, (size_t)(n - tl) * 2);
        p = d2m_ustr(s, tok);
    }
}
