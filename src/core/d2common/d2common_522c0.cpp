// D2Common 0x522C0 port.
#include <cstdint>
#include "d2common_522c0.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

// 1.13c D2Common+0x522C0
extern "C" uint32_t D2Common_522C0_StatListById(uint32_t pUnit, uint32_t nId, int32_t bCreate, const D2Common522C0Env* env)
{
	const uint32_t pSl = rd32(pUnit + 0x5C);
	uint32_t pFound = 0;
	if (pSl != 0 && static_cast<int32_t>(rd32(pSl + 0x10)) < 0) pFound = env->find(pSl, nId);
	if (bCreate != 0) {
		if (pFound == 0) {
			pFound = env->alloc(rd32(pUnit + 8), 0, 0, rd32(pUnit), rd32(pUnit + 0xC));
			if (pFound != 0) *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pFound) + 0x14) = nId;
			env->attach(pUnit, pFound, 1);
		}
		return pFound;
	}
	if (pFound != 0) {
		env->detach(pFound);
		if (static_cast<int32_t>(rd32(pFound + 0x10)) >= 0) env->free(pFound);
	}
	return 0;
}
