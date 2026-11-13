// 1.13c D2Client+0x36480 -- button-down handler of the small corner button: with the lock word clear and the cursor
// inside the button's box (measured from the screen's right and bottom edges) it latches the pressed flag and
// finishes the message.
#include "cl_pa4.h"
extern "C" uint32_t d2c_36480(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    uint32_t a = G(e, 0x11c454);
    if (a) return a;
    a = G(e, 0xdbc48) - 0x4b;
    const int32_t x = (int32_t)G(e, 0x11b828);
    const uint32_t c = G(e, 0xdbc4c) - 0xa0;
    if (x < (int32_t)a) return a;
    a += 0x23;
    if (x > (int32_t)a) return a;
    a = G(e, 0x11b824);
    if ((int32_t)a < (int32_t)(c - 0x24) || (int32_t)a > (int32_t)c) return a;
    S(e, 0x11c450, 1);
    return fin(e, msg);
}
