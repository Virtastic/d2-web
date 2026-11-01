// 1.13c D2Common+0x11430: LCG roll; level 0x40 applies the fixed records 0x96538 and (counter forced to 3) 0x96528, every other the 0x964B8 one; then the 0x96478 one (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_11430_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x11430
{
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	const uint32_t nId = D(pLevel + 0x1D0);
	if (nId == 0x40)
	{
		e->f106b0(Rec(0x96538), pLevel, &a);
		a = 3;
		e->f106b0(Rec(0x96528), pLevel, &a);
	}
	else
		Step(e, pLevel, 0x964B8, &a);
	Step(e, pLevel, 0x96478, &a);
}
