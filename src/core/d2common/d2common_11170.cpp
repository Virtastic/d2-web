// 1.13c D2Common+0x11170: roll the level LCG and apply the record at 0x96188 (+0x961C8 for level 0x81), each indexed by the counter the 0x106B0 call stepped (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_11170_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x11170
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	const uint32_t nId = D(pLevel + 0x1D0);
	Step(e, pLevel, 0x96188, &a);
	if (nId == 0x81)
		Step(e, pLevel, 0x961C8, &a);
}
