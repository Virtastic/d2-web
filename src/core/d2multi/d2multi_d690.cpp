// 1.13c D2Multi+0xD690: stdcall ret 4, arg = a wide string (a chat/command line). DAT_6f9e9c48 := -1 first. An empty string returns 0.
// If the client table [3917c] is null the fatal-message path runs (string 0x1237 and the 175D7 title to 7BD2, then exit(-1) through 14BA,
// which the capture hooks and lets return). The string is narrowed (unicode2Win, 0x100 chars) and copied; 1274(copy, &0x20) answers: zero
// sends the text to the control [3a0d0] (7C9E, EDX = 0) and returns 1, non-zero returns D5B0 run with EAX = 0 and EDI = the narrowed text.
// g: +0 DAT_6f9e9c48 (out), +4 [3917c] (only tested), +8 [3a0d0]. u2w = unicode2Win(dst, src, 0x100).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_d690(const D2mEnv& e, const uint16_t* str, uint32_t* g, void (*u2w)(char*, const uint16_t*, int)) {
    g[0] = 0xFFFFFFFFu;
    uint32_t n = 0;
    while (str[n]) n++;
    if (!n) return 0;
    if (!g[1]) {
        const uint32_t s = d2m_call(e, 0x7bde, 0, 0, 0, 0, 0, 0x1237);
        d2m_call(e, 0x7bd2, 0, 0, 0, 0, 0, D2M_MOD(0x175d7), s);
        d2m_call(e, 0x14ba, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    }
    char buf[0x100], buf2[0x100];
    uint16_t word = 0x20;
    u2w(buf, str, 0x100);
    {
        uint32_t i = 0;
        char c;
        do { c = buf[i]; buf2[i] = c; i++; } while (c);
    }
    if (!d2m_call(e, 0x1274, 0, 0, 0, 0, 0, d2m_ptr(buf2), d2m_ptr(&word))) {
        d2m_call(e, 0x7c9e, 0, g[2], 0);
        return 1;
    }
    const uint32_t r = d2m_call(e, 0xd5b0);
    d2m_edi(e, 0xd5b0, d2m_ptr(buf));
    return r;
}
