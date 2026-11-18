// 1.13c D2Client+0x49030 -- NPC menu override: three entries, the third the fixed handler with string 0xD38. Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_49030(const cl::Env& e) { return pa4::menuRec(e, 0xf3ded, 3, 0x6FAF8870u, 0xd38); }
