// 1.13c D2Client+0x42470 -- table entry with three unused stack words: two argument-free calls in a row; the second
// one's answer is returned.
#include "cl_tbl.h"
extern "C" uint32_t d2c_42470(const cl::Env& e) {
    e.H(0xd18c);
    return e.H(0xd198);
}
