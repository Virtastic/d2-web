// 1.13c D2Client+0x42170 -- key-down message: a first press (the repeat bit of the key data clear) raises the
// flag word; the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_42170(uint32_t msg, const cl::Env& e) {
    if (!(e.R(msg + 0xc) & 0x40000000u)) pa4::S(e, 0x11c38c, 1);
    return pa4::fin(e, msg);
}
