// 1.13c D2Common+0x119B0: LCG roll, the 0x96908 record, then per level 0x1D / 0x1E the 0x969C8 / 0x96A08 one; level 0x1F ends with 0x96948, every other with 0x96988 (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_119b0_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x119B0
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	const uint32_t nId = D(pLevel + 0x1D0);
	Step(e, pLevel, 0x96908, &a);
	if (nId == 0x1D)
		Step(e, pLevel, 0x969C8, &a);
	if (nId == 0x1E)
		Step(e, pLevel, 0x96A08, &a);
	if (nId == 0x1F)
		Step(e, pLevel, 0x96948, &a);
	else
		Step(e, pLevel, 0x96988, &a);
}
