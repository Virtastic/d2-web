// 1.13c D2Client+0x1C920 -- table entry: a unit of class 0x92 loses bit 1 of its flag word at +0xC4. EAX is left as
// it came in.
#include "cl_tbl.h"
extern "C" uint32_t d2c_1c920(uint32_t eax, uint32_t u, const cl::Env& e) {
    if (u && e.R(u + 4) == 0x92) e.W(u + 0xc4, e.R(u + 0xc4) & ~2u);
    return eax;
}
