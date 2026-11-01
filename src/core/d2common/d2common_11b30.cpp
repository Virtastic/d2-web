// 1.13c D2Common+0x11B30: LCG roll, the 0x96B88 record, 0x96BC8 (level 8) or 0x96C08, then 0x96C48 for level 9 and 0x96C88 for 10 (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_11b30_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x11B30
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	const uint32_t nId = D(pLevel + 0x1D0);
	Step(e, pLevel, 0x96B88, &a);
	Step(e, pLevel, nId == 8 ? 0x96BC8u : 0x96C08u, &a);
	if (nId == 9)
		Step(e, pLevel, 0x96C48, &a);
	if (nId == 10)
		Step(e, pLevel, 0x96C88, &a);
}
