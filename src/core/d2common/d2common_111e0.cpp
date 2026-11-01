// 1.13c D2Common+0x111E0: LCG roll mod 3 picks the 0x96208 record (not level 0x7C); level 0x7B then needs the counter to have moved and applies the 0x96238 one (the unmoved case is retail's fatal exit, not modelled) (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_111e0_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x111E0
{
	const uint32_t m = Lcg(pLevel) % 3;
	int32_t a = static_cast<int32_t>(m);
	const uint32_t nId = D(pLevel + 0x1D0);
	if (nId != 0x7C)
		Step(e, pLevel, 0x96208, &a);
	if (nId == 0x7B)
	{
		if (static_cast<uint32_t>(a) == m)
			return;
		Step(e, pLevel, 0x96238, &a);
	}
}
