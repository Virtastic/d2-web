// 1.13c D2Client+0x3C880 -- key command: nothing while the first check answers or the word at 0x113A68 is set. With the
// panel flag at 0xFADA4 up, close it (stack word 1); otherwise try the two-word callee (0, 1) and, when it answers 0,
// open through the last callee (EAX = 1, stack word 0).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3c880(uint32_t, const cl::Env& e) {
    uint32_t r = e.H(0x45980);
    if (r) return r;
    r = clc::G(e, 0x113A68);
    if (r) return r;
    if (clc::G(e, 0xFADA4)) return e.H(0x656b0, 1);
    r = e.H(0xbe350, 0, 1);
    if (r) return r;
    return e.H(0x65760, 0, 0, 0, 0, 0, 1);
}
