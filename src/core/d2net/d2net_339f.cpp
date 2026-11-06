// 1.13c D2Net+0x339F: the case half of the multibyte table build (called by 0x353A).
// GetCPInfo(mbcodepage) == 1: the 256-byte string src[i] = i, src[0] = ' ', every lead range [lo, hi] of the CPINFO
// (pairs from +6 up to a zero lo; lo > hi skipped) blanked to ' '; ___crtGetStringTypeA(1, src, 0x100, ctype, cp, lcid, 0)
// and __crtLCMapStringA(lcid, LCMAP_LOWERCASE / LCMAP_UPPERCASE, src, 0x100, out, 0x100, cp, 0); then per byte: C1_UPPER =
// mbctype[1+i] |= 0x10 and casemap = the lowercase byte, else C1_LOWER = |= 0x20 and the uppercase byte, else casemap 0.
// Otherwise the ASCII rule: 'A'..'Z' |= 0x10 (+0x20), 'a'..'z' |= 0x20 (-0x20), every other casemap byte 0.
// The locals keep retail's frame order (ctype, upper, lower, src: contiguous), so an out-buffer write lands as retail's does.
#include <cstdint>
#include <cstring>
#include "d2net_mbwin.h"
namespace d2net_339f {
struct Host {
    int (*GetCPInfo)(uint32_t cp, uint8_t* info);   // 20-byte CPINFO
    int (*crtGetStringTypeA)(uint32_t type, const uint8_t* src, int cch, uint16_t* out, uint32_t cp, uint32_t lcid, int err);   // 0x4F78
    int (*crtLCMapStringA)(uint32_t lcid, uint32_t flags, const uint8_t* src, int cch, uint8_t* dst, int dstlen, uint32_t cp, int err);   // 0x51C0
};
// 1.13c D2Net+0x339F
void buildCaseMap(const Host& h, d2net_mb::Window& w) {
    uint8_t ci[20];
    uint8_t f[0x500];                       // ctype words +0, upper +0x200, lower +0x300, src +0x400
    uint16_t* ctype = (uint16_t*)f;
    uint8_t* upper = f + 0x200; uint8_t* lower = f + 0x300; uint8_t* src = f + 0x400;
    if (h.GetCPInfo(w.mbcodepage, ci) == 1) {
        for (uint32_t i = 0; i < 0x100; ++i) src[i] = (uint8_t)i;
        src[0] = ' ';
        for (const uint8_t* p = ci + 6; p[0]; p += 2)
            if (p[0] <= p[1]) memset(src + p[0], ' ', (size_t)(p[1] - p[0]) + 1);
        h.crtGetStringTypeA(1, src, 0x100, ctype, w.mbcodepage, w.lcid, 0);
        h.crtLCMapStringA(w.lcid, 0x100, src, 0x100, lower, 0x100, w.mbcodepage, 0);
        h.crtLCMapStringA(w.lcid, 0x200, src, 0x100, upper, 0x100, w.mbcodepage, 0);
        for (uint32_t i = 0; i < 0x100; ++i) {
            uint16_t t; memcpy(&t, f + 2 * i, 2);
            if (t & 1) { w.mbctype[1 + i] |= 0x10; w.mbcasemap[i] = lower[i]; }
            else if (t & 2) { w.mbctype[1 + i] |= 0x20; w.mbcasemap[i] = upper[i]; }
            else w.mbcasemap[i] = 0;
        }
    } else {
        for (uint32_t i = 0; i < 0x100; ++i) {
            if (i >= 0x41 && i <= 0x5a) { w.mbctype[1 + i] |= 0x10; w.mbcasemap[i] = (uint8_t)(i + 0x20); }
            else if (i > 0x60 && i < 0x7b) { w.mbctype[1 + i] |= 0x20; w.mbcasemap[i] = (uint8_t)(i - 0x20); }
            else w.mbcasemap[i] = 0;
        }
    }
}
}
