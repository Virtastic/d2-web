// 1.13c D2Client+0x4A3A0 -- NPC menu override: four entries, the fourth the fixed handler with string 0x2BA0. Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_4a3a0(const cl::Env& e) { return pa4::menuRec(e, 0xf3ba4, 4, 0x6FAFA2A0u, 0x2ba0); }
