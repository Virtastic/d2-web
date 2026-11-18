// 1.13c D2Client+0x5C020 -- button-up handler of the expansion panel's swap button: in an expansion game, with the
// panel not in mode 2 and the click inside the button (x within 0x2E of the panel's left word, y 0x18..0x3C), it
// runs the swap check and, when that passes, sends the three-byte request {0x61, 0, 0} and finishes the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_5c020(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (!G(e, 0x119854)) return 0;
    const int32_t x = (int32_t)P16(e, msg + 0xc), y = (int32_t)P16(e, msg + 0xe);
    e.W(msg + 0x18, 0);
    const int32_t c = (int32_t)G(e, 0x11c28c);
    if (G(e, 0x11c284) == 2 || x < c || x > c + 0x2e || y < 0x18 || y > 0x3c) {
        S(e, 0x11c2a0, 0);
        return (uint32_t)x;
    }
    if (!e.H(0x5bcc0, 1)) return 0;
    const uint8_t b[3] = {0x61, 0, 0};
    send(e, b, 3);
    return fin(e, msg);
}
