// 1.13c D2Client+0x4A030 -- NPC menu override: three entries, the third the fixed handler with string 0xD45. Returns 1.
#include "cl_pa4.h"
extern "C" uint32_t d2c_4a030(const cl::Env& e) { return pa4::menuRec(e, 0xf4231, 3, 0x6FAF9440u, 0xd45); }
