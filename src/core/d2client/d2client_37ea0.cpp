// 1.13c D2Client+0x37EA0 -- forwarding method (ECX = the wrapper): tail-jumps to the entry at +0x14 of the inner
// object's method table with ECX = the inner object (the dword at +8); the stack passes through untouched.
#include "cl_tbl.h"
extern "C" uint32_t d2c_37ea0(uint32_t self, const cl::Env& e) { return clt::innerCall(e, self, 0x14); }
