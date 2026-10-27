// 1.13c D2Common+0x24DA0 index clamp: min(max(nId,0), nVarInvGfx-1), the minimum taken as signed,
// so nVarInvGfx==0 yields -1 (retail reads the slot before szInvGfx[0]).
#pragma once
#include <stdint.h>
static inline int32_t d2common_varinvgfx_index(int32_t nId, uint8_t nVarInvGfx)
{
	int32_t last = (int32_t)((uint32_t)nVarInvGfx - 1u);
	int32_t id = nId < 1 ? 0 : nId;
	return id < last ? id : last;
}
