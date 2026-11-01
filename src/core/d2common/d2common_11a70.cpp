// 1.13c D2Common+0x11A70: LCG roll, the 0x96A48 record, 0x96A88 for level 0x12, 0x96B08 for 0x13 / 0x85 and 0x96B48 for 0x15-0x18 (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_11a70_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x11A70
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	const uint32_t nId = D(pLevel + 0x1D0);
	Step(e, pLevel, 0x96A48, &a);
	if (nId == 0x12)
		Step(e, pLevel, 0x96A88, &a);
	if (nId == 0x13 || nId == 0x85)
		Step(e, pLevel, 0x96B08, &a);
	if (nId >= 0x15 && nId <= 0x18)
		Step(e, pLevel, 0x96B48, &a);
}
