// 1.13c D2Multi+0x11F80: no arguments. Refreshes the channel list control (handle DAT_6fa09cc0, with the second control DAT_6fa09cc4): g[0]=cc0, g[1]=cc4,
// g[2]=dcc (row count), g[3]=dc8 (set to dcc). Copies the dcc 0x3b-byte rows from src (module+0x1BF20) to dst (module+0x2A598), adds each row name (+1) to the
// control with 7CBC and, when Ordinal_10054() (7E2A) is nonzero, the decimal of its u16 at +0x31 as column text through 7C6E(ECX, EDX = text, item, 1).
// Afterwards the entry equal to the control's current text (7C4A, narrowed) is selected with 7CB6; 7CD4 restores the saved state (7C7A), or 0 when there was no text.
#include "d2multi_env.h"
#include <cstring>

void d2multi_11f80(const D2mEnv& e, int32_t* g, const uint8_t* src, uint8_t* dst) {
    const uint32_t cc0 = (uint32_t)g[0], cc4 = (uint32_t)g[1];
    if (!cc0) return;
    char cur[256];
    bool have = false;
    const uint16_t* t = (const uint16_t*)(uintptr_t)d2m_call(e, 0x7c4a, 0, cc0);
    if (t) {
        int n = 0;
        while (t[n] && n < 255) { cur[n] = (char)t[n]; n++; }
        cur[n] = 0;
        have = true;
    }
    uint32_t saved = d2m_call(e, 0x7c7a, 0, cc0);
    d2m_call(e, 0x7cb0, 0, cc0);
    d2m_call(e, 0x7cb0, 0, cc4);
    g[3] = g[2];
    if (!g[2]) return;
    uint32_t show = d2m_call(e, 0x7e2a) != 0;
    memcpy(dst, src, (size_t)g[2] * 0x3b);
    int32_t n = g[3];
    for (int32_t i = 0; i < n; i++) {
        const uint8_t* row = dst + i * 0x3b;
        d2m_call(e, 0x7cbc, 0, cc0, d2m_ptr(row + 1));
        uint32_t item = d2m_call(e, 0x7c74, 0, cc0);
        if (show) {
            char buf[16];
            uint32_t v = row[0x31] | (row[0x32] << 8);
            d2m_call(e, 0x1096, 0, 0, 0, 0, 0, v, d2m_ptr(buf), 10);
            d2m_call(e, 0x7c6e, 0, cc0, d2m_ptr(buf), 0, 0, item, 1);
        }
    }
    uint32_t sel = 0;
    if (have) {
        sel = saved;
        for (int32_t i = 0; i < n; i++)
            if (!strcmp(cur, (const char*)(dst + i * 0x3b + 1))) {
                d2m_call(e, 0x7cb6, 0, cc0, (uint32_t)i);
                break;
            }
    }
    d2m_call(e, 0x7cd4, 0, cc0, sel);
}
