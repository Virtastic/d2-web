// 1.13c D2Client+0x44A70 -- message handler that only runs the left-button release routine; its answer is returned.
#include "cl_pa4.h"
extern "C" uint32_t d2c_44a70(const cl::Env& e) { return e.H(0x44800); }
