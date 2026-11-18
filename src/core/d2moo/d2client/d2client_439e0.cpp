// 1.13c D2Client+0x439E0 -- message handler: clears the shift latch. While input is held off it runs the hold-off
// routine, sets the two cursor words to 0x13 and leaves the message unhandled; otherwise it sends the request,
// raises the pending word and marks the message handled. EAX is the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_439e0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    S(e, 0x11c3e8, 0);
    const uint32_t busy = G(e, 0x11c3a4);
    if (busy) {
        e.H(0xc0250, 0, 0, 0, 0, 0, busy);   // EAX still holds the flag just tested
        S(e, 0xf2188, 0x13);
        S(e, 0xf218c, 0x13);
        e.W(msg + 0x18, 0);
        return msg;
    }
    e.H(0x14660);
    S(e, 0x11c404, 1);
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1c, 0);
    return msg;
}
