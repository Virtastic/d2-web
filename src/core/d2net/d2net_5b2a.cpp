// 1.13c D2Net+0x5B2A ___isctype_mt: cdecl(ptlocinfo, c, mask).
// (unsigned)(c + 1) <= 0x100: pctype[c] & mask, pctype = the word table at ptlocinfo+0x48 (index -1 included).
// Otherwise c is a 1- or 2-byte string (2 when pctype[(c >> 8) & 0xff] has the lead bit 0x8000: {hi, lo, 0}, else
// {lo, 0}) handed to ___crtGetStringTypeA(1, s, n, out, codepage +4, lcid +0x14, 1); out is the high word of c's own
// argument slot, so the answer is that word and the 4-byte mask read back from the slot after it (a 2-word answer
// overwrites mask's low word). A zero result returns 0.
#include <cstdint>
#include <cstring>
namespace d2net_5b2a {
using GST = int (*)(uint32_t type, const uint8_t* src, int cch, uint16_t* out, uint32_t cp, uint32_t lcid, int err);
// 1.13c D2Net+0x5B2A
int isctype_mt(const uint8_t* ptloc, int c, int mask, GST gst) {
    uint32_t pct; memcpy(&pct, ptloc + 0x48, 4);
    const uint8_t* pctype = (const uint8_t*)(uintptr_t)pct;
    auto ct = [&](int i) { uint16_t v; memcpy(&v, pctype + 2 * i, 2); return v; };
    if ((uint32_t)(c + 1) <= 0x100) return ct(c) & mask;
    uint8_t frame[8]; memcpy(frame, &c, 4); memcpy(frame + 4, &mask, 4);   // the c and mask argument slots
    const uint8_t hi = (uint8_t)(c >> 8), lo = (uint8_t)c;
    uint8_t buf[4]; int n;
    if (ct(hi) & 0x8000) { buf[0] = hi; buf[1] = lo; buf[2] = 0; n = 2; }
    else { buf[0] = lo; buf[1] = 0; n = 1; }
    uint32_t cp, lcid; memcpy(&cp, ptloc + 4, 4); memcpy(&lcid, ptloc + 0x14, 4);
    if (!gst(1, buf, n, (uint16_t*)(frame + 2), cp, lcid, 1)) return 0;
    uint16_t w; uint32_t m; memcpy(&w, frame + 2, 2); memcpy(&m, frame + 4, 4);
    return (int)(w & m);
}
}
