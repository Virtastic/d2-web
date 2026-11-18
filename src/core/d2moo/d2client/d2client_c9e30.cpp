// 1.13c D2Client+0xC9E30 -- skill do: the owner-gated launch with trailing words (0x200, 1, 0).
#include "cl_skdo.h"
extern "C" uint32_t d2c_c9e30(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) { return clsk::ownerLaunch(e, u, sk, lvl, 0x200, 1, 0); }
