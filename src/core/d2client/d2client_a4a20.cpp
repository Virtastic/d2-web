// 1.13c D2Client+0xA4A20 -- timed message: with the tick (GetTickCount, through its import slot) before the panel block's first word + 0x50
// the message is dropped (the tick stays in EAX). Otherwise each listener is offered it through slot +0x14 until one answers 1, which
// marks the message handled and runs the shared tail; none did: the page routine at 0xA44F0 (EAX = the panel block, stack = the message).
#include "cl_walk.h"
extern "C" uint32_t d2c_a4a20(uint32_t msg, const cl::Env& e) {
    const uint32_t p = clc::G(e, 0x11bc00);
    const uint32_t t = e.H(0xcef5c);
    if (e.R(p) + 0x50u > t) return t;
    uint32_t a = 0;
    if (clw::walk(e, p, 0x14, msg, a)) return pa4::fin(e, msg);
    return e.H(0xa44f0, msg, 0, 0, 0, 0, p);
}
