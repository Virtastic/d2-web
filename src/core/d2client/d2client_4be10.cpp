// 1.13c D2Client+0x4BE10 -- NPC menu override: four entries, the fourth the fixed handler with string 0x58DC. Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_4be10(const cl::Env& e) { return pa4::menuRec(e, 0xf4195, 4, 0x6FAFBD00u, 0x58dc); }
