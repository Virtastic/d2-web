// 1.13c D2Multi+0xB760: a lobby packet handler (stdcall ret 0xc: stack word 1 = a split point, word 2 = the packet, whose first byte selects).
// Byte 0x0e (name insert): the wide name behind the control [3a0d0] (7C50) is narrowed (A); [39f08]+4 goes through EC20 into B; when
// len(A) + len(B) <= 0xff, A is split at the point (C = A[0..cnt), D = A[cnt..)) with 1150 (strncpy-style, cdecl), then A := sprintf(fmt 179CC, C, B, D)
// (1000), widened (win2Unicode, 0x100 chars) and sent to the control (7C9E). Returns 0.
// Byte 0x09 (cycle): when [3a08c] != 0, the index DAT_6f9e9c48 is incremented (wraps to 0 at [3a08c]) and 7C44(ctl, 392B8 + (index << 8)) runs. Returns 0.
// Any other byte returns 1.
// g: +0 [3a0d0], +4 [39f08] (tested, and +4 handed to EC20), +8 [3a08c], +12 DAT_6f9e9c48 (out). u2w = unicode2Win(dst, src, 0x100),
// w2u = win2Unicode(dst, src, 0x100).
#include "d2multi_env.h"

extern "C" uint32_t d2multi_b760(const D2mEnv& e, int32_t cnt, const uint8_t* pkt, uint32_t* g,
                                 void (*u2w)(char*, const uint16_t*, int), void (*w2u)(uint16_t*, const char*, int)) {
    const uint8_t type = pkt[0];
    if (type == 0xE) {
        const uint32_t name = d2m_call(e, 0x7c50, 0, g[0]);
        if (!name || !g[1]) return 0;
        char A[0x100], B[0x100], C[0x100], D[0x100];
        uint16_t W[0x100];
        d2m_call(e, 0xec20, g[1] + 4, 0, 0, d2m_ptr(B));
        for (int i = 0; i < 0x100; i++) W[i] = 0;
        u2w(A, (const uint16_t*)(uintptr_t)name, 0x100);
        int32_t lenA = 0, lenB = 0;
        while (A[lenA]) lenA++;
        while (B[lenB]) lenB++;
        if (lenB + lenA > 0xFF) return 0;
        d2m_call(e, 0x1150, 0, 0, 0, 0, 0, d2m_ptr(C), d2m_ptr(A), (uint32_t)cnt);
        const int32_t rest = lenA - cnt;
        C[cnt] = 0;
        d2m_call(e, 0x1150, 0, 0, 0, 0, 0, d2m_ptr(D), d2m_ptr(A + cnt), (uint32_t)rest);
        D[rest] = 0;
        const uint32_t w[5] = { d2m_ptr(A), D2M_MOD(0x179cc), d2m_ptr(C), d2m_ptr(B), d2m_ptr(D) };
        d2m_call(e, 0x1000, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3]);
        d2m_x(e, 0x1000, &w[4], 1);
        w2u(W, A, 0x100);
        d2m_call(e, 0x7c9e, 0, g[0], d2m_ptr(W));
        return 0;
    }
    if (type == 9) {
        if (g[2]) {
            int32_t idx = (int32_t)g[3] + 1;
            g[3] = (uint32_t)idx;
            if (idx >= (int32_t)g[2]) {
                idx = 0;
                g[3] = 0;
            }
            d2m_call(e, 0x7c44, 0, g[0], D2M_MOD(0x392b8 + ((uint32_t)idx << 8)));
        }
        return 0;
    }
    return 1;
}
