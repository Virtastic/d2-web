// 1.13c D2Common+0x113E0: LCG roll and the 0x96438 record (ECX = level) (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_113e0_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x113E0
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	Step(e, pLevel, 0x96438, &a);
}
