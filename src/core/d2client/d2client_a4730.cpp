// 1.13c D2Client+0xA4730 -- panel message handler: the message is marked handled (+0x18 = 1, +0x1C = 0) and passed to the shared tail, then
// each listener of the panel block's list is offered it through method slot +0x24 until one answers 1; none did: the page step with ECX = 0xFFFFFFFFu.
#include "cl_walk.h"
extern "C" uint32_t d2c_a4730(uint32_t msg, const cl::Env& e) {
    pa4::fin(e, msg);
    const uint32_t p = clw::panel(e, 0x235);
    if (!p) return 0;
    uint32_t a = 0;
    if (clw::walk(e, p, 0x24, msg, a)) return a;
    return clw::step(e, p, 0xFFFFFFFFu);
}
