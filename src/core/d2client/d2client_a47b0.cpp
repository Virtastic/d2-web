// 1.13c D2Client+0xA47B0 -- wheel message: the signed word at msg+0xA divided by 120 (truncating) is the notch count. No notches: the
// message is marked handled and passed to the shared tail. Otherwise each listener of the panel block is offered the message through
// slot +0x08 until one answers 1; none did: the page step with ECX = -1 for positive notches, +1 for negative, then the same finish as the no-notch case.
#include "cl_walk.h"
extern "C" uint32_t d2c_a47b0(uint32_t msg, const cl::Env& e) {
    const int32_t w = (int16_t)(e.R(msg + 8) >> 16);
    const int32_t n = w / 120;
    if (w == 0 || n == 0) return pa4::fin(e, msg);
    const uint32_t p = clw::panel(e, 0x1f9);
    if (!p) return 0;
    uint32_t a = 0;
    if (clw::walk(e, p, 0x8, msg, a)) return a;
    clw::step(e, p, n > 0 ? 0xFFFFFFFFu : 1u);
    return pa4::fin(e, msg);   // no listener took it: the page step falls through to the shared finish
}
