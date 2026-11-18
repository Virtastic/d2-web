// 1.13c D2Client+0xBFDE0 -- table entry: one argument-free call. Returns 1.
#include "cl_tbl.h"
extern "C" uint32_t d2c_bfde0(const cl::Env& e) {
    e.H(0xbf720);
    return 1;
}
