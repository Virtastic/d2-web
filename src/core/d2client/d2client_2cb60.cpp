// 1.13c D2Client+0x2CB60 (152 B, ret 8, EDI = dst): 0 when either argument's low word is 0x1506. Else appends
// str(a), str(0xF9D), str(0xF9B), str(b), str(0xF9E) to dst; returns 1. (The 64-unit local the retail code zeroes is never used.)
#include "d2client_ustr_env.h"
extern "C" uint32_t d2c_port_2cb60(uint16_t* dst, uint32_t a, uint32_t b, const D2ClientUStrEnv& e) {
    if ((uint16_t)a == 0x1506 || (uint16_t)b == 0x1506) return 0;
    e.strcat(dst, e.str(a));
    e.strcat(dst, e.str(0xF9D));
    e.strcat(dst, e.str(0xF9B));
    e.strcat(dst, e.str(b));
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
