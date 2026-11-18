// 1.13c D2Client+0x2C790 -- "[+]a.b" widened into dst. EAX = a, ECX = b, EDX = plus flag, stack dst (ret 4); returns 1.
// The plus sign needs plus != 0 and a >= 0.
#include "d2client_wfmt.h"
extern "C" uint32_t d2c_port_2c790(uint16_t* dst, int32_t a, int32_t b, uint32_t plus) {
    using namespace d2client_wfmt; Buf o;
    if (plus && a >= 0) put(o, '+');
    putInt(o, a); put(o, '.'); putInt(o, b);
    widen(o, dst);
    return 1;
}
