// 1.13c D2Common+0x11380: LCG roll and the 0x963B8 and 0x963F8 records (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_11380_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x11380
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	Step(e, pLevel, 0x963B8, &a);
	Step(e, pLevel, 0x963F8, &a);
}
