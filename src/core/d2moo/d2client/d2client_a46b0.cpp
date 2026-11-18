// 1.13c D2Client+0xA46B0 -- panel message handler: the message is marked handled (+0x18 = 1, +0x1C = 0) and passed to the shared tail, then
// each listener of the panel block's list is offered it through method slot +0x28 until one answers 1; none did: the page step with ECX = 1.
#include "cl_walk.h"
extern "C" uint32_t d2c_a46b0(uint32_t msg, const cl::Env& e) {
    pa4::fin(e, msg);
    const uint32_t p = clw::panel(e, 0x255);
    if (!p) return 0;
    uint32_t a = 0;
    if (clw::walk(e, p, 0x28, msg, a)) return a;
    return clw::step(e, p, 1);
}
