// 1.13c D2Common+0x50CE0.
#include <cstdint>
#include "d2common_50ce0.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

extern "C" int32_t D2Common_50CE0_FrameStep(uint32_t pUnit, const D2Common50CE0Env* env)
{
	if (pUnit == 0 || rd32(pUnit) != 0 || (rd32(pUnit + 0xC8) >> 3 & 1) == 0) return 0;
	int32_t div = 0x13;
	if (const uint32_t pItem = env->weaponItem(pUnit)) {
		div = env->weaponSpeed(pUnit, pItem);
		if (div < 1) return 0;
	}
	// cdq / idiv: signed 32-bit, the divisor is positive here
	return static_cast<int32_t>(rd32(pUnit + 0x48) & 0xFFFFFF00u) / div;
}
