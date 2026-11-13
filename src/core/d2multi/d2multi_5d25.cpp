// D2Multi 0x5D25 -- __setmbcp (MSVC 2003 CRT). cdecl (cp), SEH-framed; returns 0, -1 or 5B91's code.
// _lock(0xD) (32A6); 1BDDC := 0; cp -2/-3/-4 become GetOEMCP / GetACP / the locale ANSI code page 1BDB0 and set 1BDDC := 1. The current
// code page (3A244) answers 0. Otherwise the 0x220-byte mbc info block (3A12C) is reused while its refcount word is 0, else malloc (4CD2);
// __setmbcp_lk-style setup 5B91(cp) answering 0 snapshots the code page 3A244, 3A130, 3A128, the five words 3A250, the 0x101 ctype bytes
// 3A140 and the 0x100 case bytes 3A260 into it (+4, +8, +0xC, +0x10, +0x1C, +0x11D) and makes it 3A12C. A -1 frees a block that is not
// 3A12C. The unwind helper 5E6C (_unlock(0xD)) runs last.
// g = the module bytes 3A128..3A35F; h = [0] 1BDB0, [1] 1BDDC. Callees by RVA: IAT GetOEMCP 0x162C0, GetACP 0x1626C.
#include "d2multi_env.h"

static inline uint32_t m5d25_rd32(const uint8_t* p) { uint32_t v; __builtin_memcpy(&v, p, 4); return v; }
static inline void m5d25_wr32(uint8_t* p, uint32_t v) { __builtin_memcpy(p, &v, 4); }

// 1.13c D2Multi+0x5D25
extern "C" int32_t d2multi_setmbcp_5d25(const D2mEnv& e, uint8_t* g, uint32_t* h, int32_t cp) {
    int32_t r = -1;
    d2m_call(e, 0x32a6, 0, 0, 0, 0, 0, 0xd);
    h[1] = 0;
    if (cp == -2) { h[1] = 1; cp = (int32_t)d2m_call(e, 0x162c0); }
    else if (cp == -3) { h[1] = 1; cp = (int32_t)d2m_call(e, 0x1626c); }
    else if (cp == -4) { h[1] = 1; cp = (int32_t)h[0]; }
    if ((uint32_t)cp == m5d25_rd32(g + 0x11c)) {
        r = 0;
    } else {
        uint32_t m = m5d25_rd32(g + 4);
        if (m == 0 || m5d25_rd32((const uint8_t*)(uintptr_t)m) != 0) m = d2m_call(e, 0x4cd2, 0, 0, 0, 0, 0, 0x220);
        if (m != 0) {
            r = (int32_t)d2m_call(e, 0x5b91, 0, 0, 0, 0, 0, (uint32_t)cp);
            if (r == 0) {
                uint8_t* b = (uint8_t*)(uintptr_t)m;
                m5d25_wr32(b, 0);
                m5d25_wr32(b + 4, m5d25_rd32(g + 0x11c));
                m5d25_wr32(b + 8, m5d25_rd32(g + 8));
                m5d25_wr32(b + 0xc, m5d25_rd32(g + 0));
                for (int i = 0; i < 5; i++) { b[0x10 + 2 * i] = g[0x128 + 2 * i]; b[0x11 + 2 * i] = g[0x129 + 2 * i]; }
                for (int i = 0; i < 0x101; i++) b[0x1c + i] = g[0x18 + i];
                for (int i = 0; i < 0x100; i++) b[0x11d + i] = g[0x138 + i];
                m5d25_wr32(g + 4, m);
            }
        }
        if (r == -1 && m != m5d25_rd32(g + 4)) d2m_call(e, 0x3a59, 0, 0, 0, 0, 0, m);
    }
    d2m_call(e, 0x5e6c);
    return r;
}
