// 1.13c D2Multi+0xCF40: the click handler of the lobby list control. Stdcall ret 4, arg = message record (+4 = message id); returns 0.
// The row under the cursor (7C68 on the list control [3a038]) is -1: nothing. A left click (0x201) on the row clicked within 500 ms before
// (DAT_6f9eba70 = the last row, [3a0e4] = its tick) reads the row's name (7C50 on the name control [3a030]), narrows it and hands it to C690;
// any other message, or a click on another row / after the window, remembers row and tick (0x201 only) and copies the row's text (7C4A)
// into the name control (7C9E) and calls 7C26(0).
// g: +0 DAT_6f9eba70 (out), +4 [3a0e4] (out), +8 [3a038], +12 [3a030]. tick = GetTickCount() (read on a 0x201 message only).
// u2w = unicode2Win(dst, src, 0x100).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_cf40(const D2mEnv& e, const uint8_t* msg, uint32_t* g, uint32_t tick,
                                 void (*u2w)(char*, const uint16_t*, int)) {
    const uint32_t row = d2m_call(e, 0x7c68, 0, g[2]);
    if (row == 0xFFFFFFFFu) return 0;
    uint32_t id;
    __builtin_memcpy(&id, msg + 4, 4);
    if (id == 0x201) {
        if (row == g[0] && tick - g[1] <= 0x1F4) {
            const uint32_t name = d2m_call(e, 0x7c50, 0, g[3]);
            if (!name) return 0;
            char buf[0x100];
            u2w(buf, (const uint16_t*)(uintptr_t)name, 0x100);
            d2m_call(e, 0xc690, 0, d2m_ptr(buf));
            return 0;
        }
        g[0] = row;
        g[1] = tick;
    }
    const uint32_t txt = d2m_call(e, 0x7c4a, 0, g[2]);
    if (!txt) return 0;
    d2m_call(e, 0x7c9e, 0, g[3], txt);
    d2m_call(e, 0x7c26, 0, 0, 0, 0, 0, 0);
    return 0;
}
