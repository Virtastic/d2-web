// 1.13c D2Client+0x6F140 -- button handler of the entry box: empties the two text buffers, sets the selection word
// to -2 and, in mode 6, clears the mode's flag byte; the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_6f140(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t mode = G(e, 0x113f60);
    S16(e, 0x11fe98, 0);
    S16(e, 0x11ec40, 0);
    S(e, 0x11c02c, 0xfffffffeu);
    if (mode == 6) S8(e, 0x113f64, 0);
    return fin(e, msg);
}
