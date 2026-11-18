// 1.13c D2Client+0x46770 -- table entry: for a unit whose class has a table row (stride 0x1A8) with the first mask
// bit set in the byte at +0xD, calls the routine with EAX = that bit, EDX = 0x48, EDI = the unit. Without a unit EAX
// is left as it came in; a class outside the table returns the class; a clear bit returns 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_46770(uint32_t eax, uint32_t u, const cl::Env& e) {
    if (!u) return eax;
    const int32_t id = (int32_t)e.R(u + 4);
    if (id < 0 || id >= (int32_t)e.R(e.dt + 0xa80)) return (uint32_t)id;
    const uint32_t row = e.R(e.dt + 0xa78) + (uint32_t)id * 0x1a8;
    if (!row) return 0;
    const uint32_t v = e.R8(row + 0xd) & e.R(e.bm);
    if (!v) return 0;
    const uint32_t r = e.H(0x69660, 0, 0, 0, 0, 0, v, 0, 0x48);
    e.Edj(0x69660, u);
    return r;
}
