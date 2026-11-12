// 1.13c D2Client+0x3A8C0 -- key command: behind the common gate, step the selection forward (stack word 1).
#include "cl_cmd.h"
extern "C" uint32_t d2c_3a8c0(uint32_t, const cl::Env& e) { return clc::gated(e, 0x3d6d0, 1); }
