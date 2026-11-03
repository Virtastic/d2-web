// 1.13c D2Common+0x4F120.
#include <cstdint>
#include "d2common_4f120.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

extern "C" uint32_t D2Common_4F120_CheckCurrentRecord(uint32_t nEax, const D2Common4F120Env* env)
{
	if (env->unitCheck(nEax) == 0) return 0;
	const uint32_t pRec = env->current();
	if (pRec == 0) return 0;
	return env->recordCheck(rd32(rd32(rd32(pRec + 0x10) + 0x58) + 0x1D0)) == 0 ? 1u : 0u;
}
