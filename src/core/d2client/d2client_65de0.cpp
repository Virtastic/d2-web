// 1.13c D2Client+0x65DE0 -- panel callback: the shared routine with stack word 2 and EAX = 0; its answer is returned.
#include "cl_pa4.h"
extern "C" uint32_t d2c_65de0(const cl::Env& e) { return e.H(0x55760, 2); }
