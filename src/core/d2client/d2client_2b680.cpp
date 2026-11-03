// 1.13c D2Client+0x2B680 -- EAX = P, ECX = Q, plain ret. Builds the six-word request {0, -1, 0, P, P, 0} on the stack and calls
// 0xA1860(Q, 0, 0, 3, 0xA283, callback 0xA09F0, &req, 0); the callee fills the first words. The result is req[0], or req[2] when req[0] is 0.
#include "cl_mb2.h"
extern "C" uint32_t d2c_2b680(uint32_t p, uint32_t q, const clm::Env& e) {
    uint32_t b[6] = { 0, 0xffffffffu, 0, p, p, 0 };
    e.H(0xa1860, 0, 0, 0, 0, 0, q, 0, 0, 3, 0xa283, clm::Env::Mod(0xa09f0), (uint32_t)(uintptr_t)b);
    return b[0] ? b[0] : b[2];
}
