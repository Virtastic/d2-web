// 1.13c D2Client+0x58990 -- skill do: the offset launch, offset byte at +0xE of the monster's data block.
#include "cl_skdo.h"
extern "C" uint32_t d2c_58990(uint32_t u, int32_t sk, uint32_t lvl, const cl::Env& e) { return clsk::offsetLaunch(e, u, sk, lvl, 0xe); }
