// 1.13c D2Client+0x933E0 (112 B, EAX = out, plain ret). a = str(0xF9E); Ordinal_10744(item, 0x35) on the
// item global [0x11BC38] (zero is fatal 0xC43: Ordinal_10265 + Ordinal_10024 + _exit(-1)); 0x56330(EAX = item, ESI = out, cap 0x200)
// writes the item label into out; then strcat(out, str(0x2B48)); strcat(out, a). The global is read again for 0x56330, as retail does.
#include "d2client_ustr_env.h"
struct D2c933e0Env {
    const D2ClientUStrEnv* u;
    uint32_t (*ord10744)(uint32_t item, uint32_t stat);              // thunk 0xC1CC
    void (*label)(uint32_t item, uint16_t* out, uint32_t cap);       // 0x56330
    void (*fatal)(uint32_t msgId);                                   // never returns
    const uint32_t* item;                                            // D2Client+0x11BC38
};
extern "C" void d2c_port_933e0(uint16_t* out, const D2c933e0Env& e) {
    const uint16_t* a = e.u->str(0xF9E);
    if (!e.ord10744(*e.item, 0x35)) e.fatal(0xC43);
    e.label(*e.item, out, 0x200);
    e.u->strcat(out, e.u->str(0x2B48));
    e.u->strcat(out, a);
}
