// 1.13c D2Client+0x2CE10 (263 B, ret 0x14, EDI = dst; stack s0..s4): s2 == 0 returns 0. Else wsprintfA(local, fmt, s2) with "+%d" when
// s3 != 0 && s2 >= 0, else "%d", widened by toUnicode. A low word of s0 other than 0x1506 first appends str(s0), str(0xF9D), str(0xF9B).
// Then local; s4 != 0 adds str(0xFA1); then str(0xF9B), str(s1), str(0xF9E); returns 1.
#include "d2client_ustr_env.h"
#include "d2client_wfmt.h"
extern "C" uint32_t d2c_port_2ce10(uint16_t* dst, uint32_t s0, uint32_t s1, int32_t s2, uint32_t s3, uint32_t s4, const D2ClientUStrEnv& e) {
    using namespace d2client_wfmt;
    if (s2 == 0) return 0;
    Buf o; if (s3 && s2 >= 0) put(o, '+');
    putInt(o, s2);
    uint16_t local[64] = {}; widen(o, local);
    if ((uint16_t)s0 != 0x1506) {
        e.strcat(dst, e.str(s0));
        e.strcat(dst, e.str(0xF9D));
        e.strcat(dst, e.str(0xF9B));
    }
    e.strcat(dst, local);
    if (s4) e.strcat(dst, e.str(0xFA1));
    e.strcat(dst, e.str(0xF9B));
    e.strcat(dst, e.str(s1));
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
