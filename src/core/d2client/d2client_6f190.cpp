// 1.13c D2Client+0x6F190 -- button handler of the entry box: raises the mode's flag byte, empties the two text
// buffers and picks the follow-up step (4 in mode 9, 5 in mode 8); the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_6f190(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t mode = G(e, 0x113f60);
    S8(e, 0x113f64, 1);
    S16(e, 0x11fe98, 0);
    S16(e, 0x11ec40, 0);
    if (mode == 9) S(e, 0x11c030, 4);
    else if (mode == 8) S(e, 0x11c030, 5);
    return fin(e, msg);
}
