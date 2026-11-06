// 1.13c D2Multi+0x4D6B ___crtGetStringTypeA: cdecl (infoType, src, cch, out, cp, lcid, bError).
// gv = the module words 1BD64 (API mode: 0 unknown, 1 wide, 2 ANSI), 1BDA0 (locale), 1BDB0 (ANSI code page). Every callee goes through env:
// the IAT slots GetStringTypeW 0x162D0, GetLastError 0x16348, MultiByteToWideChar 0x16270, GetStringTypeA 0x162D4, and memset 5340,
// calloc 3ACA, free 3A59, the locale code page lookup 6530, the string conversion 6577 and the stack-overflow reset 6751.
#include <cstdint>
#include "d2multi_env.h"

extern "C" int32_t d2multi_crtgetstringtypea_4d6b(const D2mEnv& e, uint32_t* gv, uint32_t infoType, uint32_t src, int32_t cch,
                                                   uint32_t out, uint32_t cp, uint32_t lcid, int32_t bError) {
    if (gv[0] == 0) {   // probe the wide API once
        uint32_t probe = 0;
        if (d2m_call(e, 0x162d0, 0, 0, 0, 0, 0, 1, D2M_MOD(0x166c4), 1, d2m_ptr(&probe))) gv[0] = 1;
        else if (d2m_call(e, 0x16348) == 0x78) gv[0] = 2;
    }
    const uint32_t mode = gv[0];
    if (mode == 2 || mode == 0) {
        if (!lcid) lcid = gv[1];
        if (!cp) cp = gv[2];
        const uint32_t lcp = d2m_call(e, 0x6530, 0, 0, 0, 0, 0, lcid);
        if (lcp == 0xffffffffu) return 0;
        uint32_t cvt = 0; int32_t n = cch;
        if (lcp != cp) {
            cvt = d2m_call(e, 0x6577, 0, 0, 0, 0, 0, cp, lcp, src, d2m_ptr(&n));   // + (0, 0)
            if (!cvt) return 0;
            src = cvt;
        }
        const int32_t r = (int32_t)d2m_call(e, 0x162d4, 0, 0, 0, 0, 0, lcid, infoType, src, (uint32_t)n);   // + out
        if (cvt) d2m_call(e, 0x3a59, 0, 0, 0, 0, 0, cvt);
        return r;
    }
    if (mode != 1) return 0;
    if (!cp) cp = gv[2];
    const uint32_t n = d2m_call(e, 0x16270, 0, 0, 0, 0, 0, cp, bError ? 9u : 1u, src, (uint32_t)cch);   // + (0, 0)
    if (!n) return 0;
    uint8_t stk[0x1000];   // retail: _alloca((2n + 3) & ~3); an overflow lands in its SEH handler, which resets the guard page (6751)
    uint32_t buf = 0, heap = 0;
    if (((2u * n + 3u) & ~3u) <= sizeof stk) { buf = d2m_ptr(stk); d2m_call(e, 0x5340, 0, 0, 0, 0, 0, buf, 0, 2u * n); }
    else d2m_call(e, 0x6751);
    if (!buf) {
        buf = d2m_call(e, 0x3aca, 0, 0, 0, 0, 0, 2, n);
        if (!buf) return 0;
        heap = 1;
    }
    int32_t r = 0;
    const uint32_t n2 = d2m_call(e, 0x16270, 0, 0, 0, 0, 0, cp, 1, src, (uint32_t)cch);   // + (buf, n)
    if (n2) r = (int32_t)d2m_call(e, 0x162d0, 0, 0, 0, 0, 0, infoType, buf, n2, out);
    if (heap) d2m_call(e, 0x3a59, 0, 0, 0, 0, 0, buf);
    return r;
}
