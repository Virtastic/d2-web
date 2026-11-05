// 1.13c D2Client+0x2CC00 (282 B, EDI = dst, stack a0..a4, ret 0x14): 0 when a0's low word is 0x1506, a2 == 0 or a3 <= 0.
// Else appends str(a0), str(0xF9D), str(0xF9B), the 0x2C7E0 text of a2 (plus flag a4), str(0xFA0), "%d" of a3, str(0xF9B),
// str(a1), str(0xF9E) to dst; returns 1.
#include "d2client_ustr_env.h"
#include "d2client_wfmt.h"
extern "C" uint32_t d2c_port_2c7e0(uint16_t* dst, int32_t a, uint32_t plus);
extern "C" uint32_t d2c_port_2cc00(uint16_t* dst, uint32_t a0, uint32_t a1, int32_t a2, int32_t a3, uint32_t a4, const D2ClientUStrEnv& e) {
    if ((uint16_t)a0 == 0x1506 || a2 == 0 || a3 <= 0) return 0;
    uint16_t local[32] = {};
    e.strcat(dst, e.str(a0));
    e.strcat(dst, e.str(0xF9D));
    e.strcat(dst, e.str(0xF9B));
    d2c_port_2c7e0(local, a2, a4);
    e.strcat(dst, local);
    e.strcat(dst, e.str(0xFA0));
    d2client_wfmt::Buf o; d2client_wfmt::putInt(o, a3); d2client_wfmt::widen(o, local);
    e.strcat(dst, local);
    e.strcat(dst, e.str(0xF9B));
    e.strcat(dst, e.str(a1));
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
