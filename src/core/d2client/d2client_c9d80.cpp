// 1.13c D2Client+0xC9D80 -- skill do: the owner-gated launch with trailing words (0x100, 0, 1).
#include "cl_skdo.h"
extern "C" uint32_t d2c_c9d80(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) { return clsk::ownerLaunch(e, u, sk, lvl, 0x100, 0, 1); }
