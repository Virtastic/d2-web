// 1.13c D2Client+0x3EFD0 -- consumes a pending step on a two-way control (ECX = control, one ignored stack word, ret 4):
// with the pending word (+0x30) clear it answers 0; otherwise it clears +0x40 and +0x30, then answers 1 after clearing
// the "up" flag (+0x34) if that was exactly 1, else after clearing the "down" flag (+0x38) if that was exactly 1, else 0.
#include "cl_env.h"
extern "C" uint32_t d2c_3efd0(uint32_t self, const cl::Env& e) {
    if (e.R(self + 0x30) == 0) return 0;
    const uint32_t up = e.R(self + 0x34);
    e.W(self + 0x40, 0);
    e.W(self + 0x30, 0);
    if (up == 1) {
        e.W(self + 0x34, 0);
        return 1;
    }
    if (e.R(self + 0x38) == 1) {
        e.W(self + 0x38, 0);
        return 1;
    }
    return 0;
}
