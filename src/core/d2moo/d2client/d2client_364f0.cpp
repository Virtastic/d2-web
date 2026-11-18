// 1.13c D2Client+0x364F0 -- button-down handler of the panel close button: the box depends on the resolution mode
// (mode 2: x 0x2AD..0x2CD, otherwise 0x20D..0x22D; y 0x18..0x38). Inside it latches the pressed flag; outside it
// drops the four button latches. Either way the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_364f0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const int32_t y = (int32_t)P16(e, msg + 0xe), x = (int32_t)P16(e, msg + 0xc);
    const int32_t lo = e.H(0xd0ae) == 2 ? 0x2ad : 0x20d;
    if (x >= lo && x <= lo + 0x20 && y >= 0x18 && y <= 0x38) {
        S(e, 0x11c448, 1);
        return fin(e, msg);
    }
    clearButtons(e);
    return fin(e, msg);
}
