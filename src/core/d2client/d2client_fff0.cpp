// 1.13c D2Client+0xFFF0 -- table entry: hands EDX to the one-argument release routine (caller cleans the stack) and
// returns its answer.
#include "cl_tbl.h"
extern "C" uint32_t d2c_fff0(uint32_t edx, const cl::Env& e) { return e.H(0x4891, edx); }
