// 1.13c D2Multi+0xBEB0: the lobby "quiet / sound" toggle. Stdcall ret 4, the argument is unused; returns 1. When [39f28] is set it is cleared
// (7C86 on the control [39f84], EDX = 0) and string 0x14d7 is shown; otherwise 7C86(.., 2) sets it and the shown string is 0x14d8 on a 1-in-1000
// roll of 1A65, else 0x14d6; the third and fourth 1-in-1000 rolls (both 0) show a fixed 12-char wide text instead. The string goes to 7CE0
// on the control [3a0c8] (when set) with the kind 0xb (text from D2Lang 7EA8) or 9 (the fixed text).
// g: +0 [39f28] (out), +4 [39f84], +8 [3a0c8], +12 the 12-byte text at 177F8. toU = toUnicode(dst, src, 12).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_beb0(const D2mEnv& e, uint32_t* g, void (*toU)(uint16_t*, const char*, int)) {
    uint32_t id;
    if (g[0]) {
        d2m_call(e, 0x7c86, 0, g[1], 0);
        g[0] = 0;
        id = 0x14d7;
    } else {
        d2m_call(e, 0x7c86, 0, g[1], 2);
        g[0] = 1;
        if ((int32_t)d2m_call(e, 0x1a65) % 1000 == 0) {
            id = 0x14d8;
        } else if ((int32_t)d2m_call(e, 0x1a65) % 1000 != 0) {
            id = 0x14d6;
        } else if ((int32_t)d2m_call(e, 0x1a65) % 1000 != 0) {
            id = 0x14d6;
        } else {
            uint16_t buf[20] = {};
            toU(buf, (const char*)(g + 3), 12);
            if (g[2]) d2m_call(e, 0x7ce0, 0, g[2], d2m_ptr(buf), 0, 0, 9, 0, 1);
            return 1;
        }
    }
    const uint32_t s = d2m_call(e, 0x7ea8, 0, id);
    if (g[2]) d2m_call(e, 0x7ce0, 0, g[2], s, 0, 0, 0xb, 0, 1);
    return 1;
}
