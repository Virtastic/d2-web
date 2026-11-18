// 1.13c D2Client+0x2D6C0 -- table entry with seven stack words: forwards to the four-word routine with
// (EDX, word 0, word 3, word 4), EAX = word 6 and ESI = ECX; its answer is returned.
#include "cl_tbl.h"
extern "C" uint32_t d2c_2d6c0(uint32_t ecx, uint32_t edx, uint32_t s0, uint32_t s3, uint32_t s4, uint32_t s6, const cl::Env& e) {
    return e.H(0x2c4f0, edx, s0, s3, s4, 0, s6, 0, 0, 0, ecx);
}
