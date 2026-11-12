// 1.13c D2Client+0x45E30 -- table entry: a unit of class 0x13E in mode 2 raises the byte flag at 0x11C33E. EAX is
// left as it came in.
#include "cl_tbl.h"
extern "C" uint32_t d2c_45e30(uint32_t eax, uint32_t u, const cl::Env& e) {
    if (u && e.R(u + 4) == 0x13e && e.R(u + 0x10) == 2) clt::S8(e, 0x11C33E, 1);
    return eax;
}
