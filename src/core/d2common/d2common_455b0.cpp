// 1.13c D2Common+0x455B0. The three failure ids (0x25C, 0x25E, 0x25F) end in a fatal exit and are not capturable.
#include <cstdint>
#include "d2common_455b0.h"

static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(static_cast<uintptr_t>(a)); }

extern "C" uint32_t D2Common_455B0_UnitWord1D0(uint32_t nEcx, uint32_t nEdx, uint32_t nArg, const D2Common455B0Env* env)
{
	uint32_t slot = nEcx, arg = nArg;
	const uint32_t pRec = env->lookup(nArg, nEdx, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&slot)), static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&arg)));
	if (pRec == 0 || rd32(pRec + 0x10) == 0 || rd32(rd32(pRec + 0x10) + 0x58) == 0) __builtin_trap(); // retail: fatal message, then exit
	return rd32(rd32(rd32(pRec + 0x10) + 0x58) + 0x1D0);
}
