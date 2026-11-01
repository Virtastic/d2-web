// 1.13c D2Common+0x114C0: the first room whose preset id is above 0x1AC (else the last) gives the start counter {3, 1, 0, 2}[id - 0x1BC]; then the 0x96548-0x96708 records chosen by the level id (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_114c0_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x114C0
{
	uint32_t pDef = 0;
	for (uint32_t r = D(pLevel + 0x10); r; r = D(r + 0x24))
	{
		pDef = D(r + 0x20);
		if (static_cast<int32_t>(D(pDef)) > 0x1AC)
			break;
	}
	const uint32_t nKey = D(pDef) - 0x1BC;
	if (nKey > 3)
		return; // retail: fatal exit
	static const int32_t kStart[4] = { 3, 1, 0, 2 };
	int32_t a = kStart[nKey];
	const uint32_t nId = D(pLevel + 0x1D0);
	const uint32_t pOuter = D(pLevel + 0x1B4);
	if (nId >= 0x37 && nId <= 0x3A)
		Step(e, pLevel, 0x96548, &a);
	if (nId == 0x39)
		Step(e, pLevel, 0x96588, &a);
	if (nId == 0x3B || nId == 0x3D)
		Step(e, pLevel, 0x965C8, &a);
	if (nId >= 0x42 && nId <= 0x48 && nId != D(pOuter + 0x94))
		Step(e, pLevel, 0x965C8, &a);
	if (nId == 0x3B)
		Step(e, pLevel, 0x96608, &a);
	if (nId == 0x3C)
		Step(e, pLevel, 0x96648, &a);
	if (nId == 0x3B || nId == 0x3D)
		Step(e, pLevel, 0x96688, &a);
	if (nId == D(pOuter + 0x94))
		Step(e, pLevel, 0x966C8, &a);
	if (nId == D(pOuter + 0x484))
		Step(e, pLevel, 0x96708, &a);
}
