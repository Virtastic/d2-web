// 1.13c D2Client+0xA4D80 -- table entry: the one-word routine with ECX on the stack; its answer is returned.
#include "cl_tbl.h"
extern "C" uint32_t d2c_a4d80(uint32_t ecx, const cl::Env& e) { return e.H(0xc6c4, ecx); }
