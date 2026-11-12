// 1.13c D2Client+0x67030 -- forwarding method (ECX = the wrapper): tail-jumps to the entry at +0xC of the inner
// object's method table with ECX = the inner object (the dword at +8); the stack passes through untouched.
#include "cl_tbl.h"
extern "C" uint32_t d2c_67030(uint32_t self, const cl::Env& e) { return clt::innerCall(e, self, 0xc); }
