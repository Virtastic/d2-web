// 1.13c D2Client+0x421E0 -- key-down message: a first press (the repeat bit of the key data clear) steps the byte
// counter; the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_421e0(uint32_t msg, const cl::Env& e) {
    if (!(e.R(msg + 0xc) & 0x40000000u)) pa4::S8(e, 0x11c3c8, pa4::G8(e, 0x11c3c8) + 1);
    return pa4::fin(e, msg);
}
