// 1.13c D2Client+0xBFEB0 -- table entry: one argument-free call. Returns 0.
#include "cl_tbl.h"
extern "C" uint32_t d2c_bfeb0(const cl::Env& e) {
    e.H(0xbf720);
    return 0;
}
