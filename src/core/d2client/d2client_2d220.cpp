// 1.13c D2Client+0x2D220 (213 B, EAX = sel, stack dst, low, high, kind, ret 0x10): a zero low word of sel takes 0x2C870(kind);
// a zero low word again returns 0. The range text is "%d" of low when low == high (both zero included), else "%d-%d".
// Appends str(sel, the full 32-bit word), the range, str(0xF9E) to dst; returns 1.
#include "d2client_ustr_env.h"
#include "d2client_wfmt.h"
uint32_t D2Client_Map2C870(uint32_t n);
extern "C" uint32_t d2c_port_2d220(uint32_t sel, uint16_t* dst, int32_t low, int32_t high, uint32_t kind, const D2ClientUStrEnv& e) {
    uint16_t local[32] = {};
    if ((uint16_t)sel == 0) { sel = D2Client_Map2C870(kind); if ((uint16_t)sel == 0) return 0; }
    d2client_wfmt::Buf o; d2client_wfmt::putInt(o, low);
    if (low != high) { d2client_wfmt::put(o, '-'); d2client_wfmt::putInt(o, high); }
    d2client_wfmt::widen(o, local);
    e.strcat(dst, e.str(sel));
    e.strcat(dst, local);
    e.strcat(dst, e.str(0xF9E));
    return 1;
}
