// 1.13c D2Client+0x2C9F0 (144 B, ret 8, EBX = dst): a low word of 0x1506 is rejected (0). Else local[256] zeroed;
// sprintf(256, local, str(sel), value); strcat(dst, local); strcat(dst, str(0xF9E)); returns 1.
#include "d2client_ustr_env.h"
extern "C" uint32_t d2c_port_2c9f0(uint16_t* dst, uint32_t sel, uint32_t value, const D2ClientUStrEnv& e) {
    if ((uint16_t)sel == 0x1506) return 0;
    uint16_t local[256] = {};
    e.sprintf(256, local, e.str(sel), (int32_t)value);
    e.strcat(dst, local);
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
