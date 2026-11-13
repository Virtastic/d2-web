// 1.13c D2Client+0x3EF50 -- panel-object forwarder: ECX := this[2], tail jump to child vtable slot 0xc (cl_vthunk.h).
#include "cl_vthunk.h"
extern "C" uint32_t d2c_3ef50(uint32_t self, const cl::Env& e) { return clc::vforward(e, self, 0xc); }
