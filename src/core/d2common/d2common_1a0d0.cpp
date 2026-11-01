// 1.13c D2Common+0x1A0D0: the room reached through a warp pair (retail-layout seam; the engine's DRLGWARP_GetDestinationRoom is the vendored form).
// fastcall(nSourceLevel ecx, pDrlgRoom edx, int* pDestLevel, void** ppLvlWarp), ret 8. A room's tile list is at +0x4C, a tile is {pDrlgRoom +0,
// pNext +4, pLvlWarp +0x10}, a level-warp row starts with its level id, a room's activated Room1 is at +0x30; 0x7C270 activates a room (EAX = room).
#include <cstdint>
#include "d2common_fog_env.h"

struct D2CommonWarpEnv { void (*activateRoom)(uint32_t pDrlgRoom); };

extern "C" uint32_t D2Common_1A0D0_GetDestinationRoom(int32_t nSourceLevel, uint32_t pDrlgRoom, uint32_t pDestLevel, uint32_t ppLvlWarp, const D2CommonWarpEnv* env)
{
	using d2raw::P; using d2raw::W;
	for (uint32_t t = W(P(pDrlgRoom), 0x4C); t; t = W(P(t), 4))
	{
		if (static_cast<int32_t>(W(P(W(P(t), 0x10)), 0)) != nSourceLevel) continue;
		const uint32_t pSrcRoom = W(P(t), 0);
		for (uint32_t u = W(P(pSrcRoom), 0x4C); u; u = W(P(u), 4))
		{
			if (W(P(u), 0) != pDrlgRoom) continue;
			W(P(pDestLevel), 0) = W(P(W(P(u), 0x10)), 0);
			W(P(ppLvlWarp), 0) = W(P(u), 0x10);
			if (!W(P(pSrcRoom), 0x30)) env->activateRoom(pSrcRoom);
			return W(P(pSrcRoom), 0x30);
		}
		return 0;
	}
	return 0;
}
