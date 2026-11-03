// 1.13c D2Client+0x2DE00 (326 B, ret 0x14, EBX = scale; stack dst, selA, selB, p4, p5): 24.8 fixed-point range text.
// int(p) = trunc(p / 256); frac(p) = trunc(((p % 256) * scale) / 256) with the C remainder. Both ints and fracs zero returns 0;
// ints (not both zero) with both fracs zero give "%d-%d"; else "%d.%d-%d.%d". A selA low word other than 0x1506 prefixes str(selA); local;
// selB likewise suffixes str(selB); then str(0xF9E); returns 1.
#include "d2client_ustr_env.h"
#include "d2client_wfmt.h"
namespace {
inline int32_t div256(int32_t v) { return (v + ((v >> 31) & 0xff)) >> 8; }
inline int32_t frac256(int32_t p, int32_t scale) {
    int32_t r = (int32_t)((uint32_t)p & 0x800000ffu);   // retail: p & 0x800000ff, negative case sign-extended to the C remainder
    if (r < 0) r = (int32_t)((((uint32_t)r - 1u) | 0xffffff00u) + 1u);
    return div256((int32_t)((uint32_t)r * (uint32_t)scale));
}
}
extern "C" uint32_t d2c_port_2de00(uint16_t* dst, int32_t scale, uint32_t selA, uint32_t selB, int32_t p4, int32_t p5, const D2ClientUStrEnv& e) {
    using namespace d2client_wfmt;
    int32_t li = div256(p4), lf = frac256(p4, scale), hi = div256(p5), hf = frac256(p5, scale);
    Buf o;
    if (li == 0 && hi == 0) {
        if (lf == 0 && hf == 0) return 0;
        putInt(o, li); put(o, '.'); putInt(o, lf); put(o, '-'); putInt(o, hi); put(o, '.'); putInt(o, hf);
    } else if (lf == 0 && hf == 0) {
        putInt(o, li); put(o, '-'); putInt(o, hi);
    } else {
        putInt(o, li); put(o, '.'); putInt(o, lf); put(o, '-'); putInt(o, hi); put(o, '.'); putInt(o, hf);
    }
    uint16_t local[64] = {}; widen(o, local);
    if ((uint16_t)selA != 0x1506) e.strcat(dst, e.str(selA));
    e.strcat(dst, local);
    if ((uint16_t)selB != 0x1506) e.strcat(dst, e.str(selB));
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
