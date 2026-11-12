// 1.13c D2Client+0x37E20 -- forwarding method (ECX = the wrapper, one stack word that is dropped): calls the entry at
// +0x34 of the inner object's method table with ECX = the inner object and no stack words of its own.
#include "cl_tbl.h"
extern "C" uint32_t d2c_37e20(uint32_t self, const cl::Env& e) { return clt::innerCall(e, self, 0x34); }
