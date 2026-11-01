// 1.13c D2Common+0x11940: LCG roll, the 0x96888 record and for level 0x23 the 0x968C8 one (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_11940_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x11940
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	const uint32_t nId = D(pLevel + 0x1D0);
	Step(e, pLevel, 0x96888, &a);
	if (nId == 0x23)
		Step(e, pLevel, 0x968C8, &a);
}
