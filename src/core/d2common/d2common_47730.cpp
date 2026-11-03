// 1.13c D2Common+0x47730.
#include <cstdint>
#include "d2common_47730.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

extern "C" void D2Common_47730_SeedItem(uint32_t pUnit, uint32_t nArg, const D2Common47730Env* env)
{
	if (pUnit == 0) return;
	env->prep(pUnit, nArg);
	if (rd32(pUnit) == 4 && rd32(pUnit + 0x14) != 0 && (rd32(rd32(pUnit + 0x14) + 0x18) & 0x400000) != 0)
		env->seed(pUnit);
}
