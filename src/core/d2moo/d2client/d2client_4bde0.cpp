// 1.13c D2Client+0x4BDE0 -- NPC menu override: five entries, the fifth the fixed handler with string 0x58DD. Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_4bde0(const cl::Env& e) { return pa4::menuRec(e, 0xf41e3, 5, 0x6FAFBD00u, 0x58dd); }
