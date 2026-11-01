// 1.13c D2Common+0x1A6D0: store a copy of a monster's name in its MonsterData (retail-layout seam; the engine's own
// MONSTERS_SetMonsterNameInMonsterData is the vendored form). fastcall(pUnit ecx, wszName edx).
#include <cstdint>
#include "d2common_fog_env.h"

extern "C" void D2Common_1A6D0_SetMonsterName(uint32_t pUnit, uint32_t pName, const D2CommonFogEnv* env)
{
	using d2raw::P; using d2raw::W;
	uint8_t* u = P(pUnit);
	if (!u || W(u, 0) != 1 || !W(u, 0x14) || !pName) return;
	const uint16_t* src = reinterpret_cast<const uint16_t*>(P(pName));
	uint32_t n = 0;
	while (src[n]) ++n;
	const uint32_t pBuf = env->strAlloc(n * 2 + 2);   // Fog alloc, source line 0xC3
	W(P(W(u, 0x14)), 0x2C) = pBuf;
	uint16_t* dst = reinterpret_cast<uint16_t*>(P(pBuf));
	for (uint32_t i = 0; i <= n; ++i) dst[i] = src[i];
}
