// 1.13c D2Client+0x49060 -- NPC menu override: three entries, the third the fixed handler with string 0xD36. Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_49060(const cl::Env& e) { return pa4::menuRec(e, 0xf3d9f, 3, 0x6FAF88D0u, 0xd36); }
