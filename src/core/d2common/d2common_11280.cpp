// 1.13c D2Common+0x11280: LCG roll, the 0x96278 record, then for level 0x65 the 0x962B8 and 0x962F8 ones and for 0x64 the 0x962F8 one (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_11280_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x11280
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	const uint32_t nId = D(pLevel + 0x1D0);
	Step(e, pLevel, 0x96278, &a);
	if (nId == 0x65)
	{
		Step(e, pLevel, 0x962B8, &a);
		Step(e, pLevel, 0x962F8, &a);
	}
	if (nId == 0x64)
		Step(e, pLevel, 0x962F8, &a);
}
