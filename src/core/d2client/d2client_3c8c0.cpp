// 1.13c D2Client+0x3C8C0 -- key command: behind the common gate, one callee with stack word 1.
#include "cl_cmd.h"
extern "C" uint32_t d2c_3c8c0(uint32_t, const cl::Env& e) { return clc::gated(e, 0x1e940, 1); }
