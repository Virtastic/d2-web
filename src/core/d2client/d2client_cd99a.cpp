// 1.13c D2Client+0xCD99A: SEH unwind funclet. EAX = [EBP+4] + 0x50, then a bare tail jump to the 0x1E70 helper (push eax; call [import]).
#include "cl_env.h"
extern "C" void d2c_cd99a(uint32_t ebp, const cl::Env& e) { e.H(0x1e70, 0, 0, 0, 0, 0, e.R(ebp + 4) + 0x50u); }
