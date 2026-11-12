// 1.13c D2Client+0xA0740 -- visitor: counts a visit in the dword the record's pointer at +8 names. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_a0740(uint32_t rec, const cl::Env& e) {
    const uint32_t p = e.R(rec + 8);
    e.W(p, e.R(p) + 1);
    return 1;
}
