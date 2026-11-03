// 1.13c D2Common+0x4EBF0.
#include <cstdint>
#include "d2common_4ebf0.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

extern "C" uint32_t D2Common_4EBF0_SlotWord(uint32_t nMode, uint32_t pRec, const D2Common4EBF0Env* env)
{
	if (pRec == 0 || rd32(pRec) != 0x1020304u) return 0;
	const uint32_t pTbl = env->table(0x9D378u);
	if (pTbl == 0) return 0;
	return rd32(rd32(pTbl + 0xC) + (nMode == 5 ? 0x10u : 0x14u));
}
