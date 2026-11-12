// 1.13c D2Client+0x511D0 -- table predicate: 1 for a unit of type 4 whose class passes the one-word check, else 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_511d0(uint32_t u, const cl::Env& e) {
    if (!u || e.R(u) != 4) return 0;
    return e.H(0xccee, e.R(u + 4)) ? 1 : 0;
}
