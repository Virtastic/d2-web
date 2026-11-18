// 1.13c D2Client+0x65E30 -- calls the panel-loop routine 0x65760 with EAX = 0 and the screen mode 1; its answer stays in EAX.
#include "cl_env.h"
extern "C" uint32_t d2c_65e30(const cl::Env& e) { return e.H(0x65760, 1); }
