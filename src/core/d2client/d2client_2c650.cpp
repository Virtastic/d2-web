// 1.13c D2Client+0x2C650 -- damage range text "[+]a.b-c.d" widened into dst (ESI,EDX,ECX,EAX = a,b,c,d; stack dst, plus; ret 8).
#include "d2client_wfmt.h"
extern "C" void d2c_port_2c650(uint16_t* dst, uint32_t plus, int32_t a, int32_t b, int32_t c, int32_t d) {
    using namespace d2client_wfmt; Buf o;
    if (plus) put(o, '+');
    putInt(o, a); put(o, '.'); putInt(o, b); put(o, '-'); putInt(o, c); put(o, '.'); putInt(o, d);
    widen(o, dst);
}
