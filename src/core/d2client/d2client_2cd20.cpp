// 1.13c D2Client+0x2CD20 (227 B, ret 0x14, EBX = sel, EDI = dst; stack a0..a4): a low word of 0x1506 in EBX, or a1 == 0 && a2 == 0, returns 0.
// Else local[64] zeroed; 0x2C790(eax=a1, ecx=a2, edx=a3, local) builds "[+]a.b"; strcat dst str(sel), str(0xF9D), str(0xF9B), local;
// a4 != 0 adds str(0xFA1); then str(0xF9B), str(a0), str(0xF9E); returns 1.
#include "d2client_ustr_env.h"
extern "C" uint32_t d2c_port_2c790(uint16_t* dst, int32_t a, int32_t b, uint32_t plus);
extern "C" uint32_t d2c_port_2cd20(uint16_t* dst, uint32_t sel, uint32_t a0, int32_t a1, int32_t a2, uint32_t a3, uint32_t a4, const D2ClientUStrEnv& e) {
    if ((uint16_t)sel == 0x1506 || (a1 == 0 && a2 == 0)) return 0;
    uint16_t local[64] = {};
    d2c_port_2c790(local, a1, a2, a3);
    e.strcat(dst, e.str(sel));
    e.strcat(dst, e.str(0xF9D));
    e.strcat(dst, e.str(0xF9B));
    e.strcat(dst, local);
    if (a4) e.strcat(dst, e.str(0xFA1));
    e.strcat(dst, e.str(0xF9B));
    e.strcat(dst, e.str(a0));
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
