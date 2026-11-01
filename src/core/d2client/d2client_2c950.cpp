// 1.13c D2Client+0x2C950 (153 B, ret 8): a text popup. buf[100] zeroed; sprintf(100, buf, str(0x1058), a); strcat str(0xF9E);
// strcat str(b); popup(buf, edx = [DBC48] - 0x80, [DBC4C] - 0x34, 0, 1).
#include "d2client_ustr_env.h"
extern "C" void d2c_port_2c950(uint32_t a, uint32_t b, const D2ClientUStrEnv& e) {
    uint16_t buf[100] = {};
    e.sprintf(100, buf, e.str(0x1058), (int32_t)a);
    e.strcat(buf, e.str(0xF9E));
    e.strcat(buf, e.str(b));
    e.popup(buf, e.g48 - 0x80u, e.g4c - 0x34u, 0, 1);
}
