// 1.13c D2Client+0x2C7E0 -- "[+]a" widened into dst. EAX = a, ECX = plus flag, stack dst (ret 4); returns 1.
#include "d2client_wfmt.h"
extern "C" uint32_t d2c_port_2c7e0(uint16_t* dst, int32_t a, uint32_t plus) {
    using namespace d2client_wfmt; Buf o;
    if (plus && a >= 0) put(o, '+');
    putInt(o, a);
    widen(o, dst);
    return 1;
}
