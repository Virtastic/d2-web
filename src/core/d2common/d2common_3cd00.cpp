// 1.13c D2Common+0x3CD00 (#10264): the warp tile a room's warp to a source level arrives on
// (retail-layout seam; the engine's DUNGEON_GetWarpTileFromRoomAndSourceLevelId in d2moo/src/D2Dungeon.cpp is the vendored form).
// stdcall(pRoom, nSourceLevel, ppLvlWarp), ret 0xC. 0x1A0D0 on the room's DRLG room (+0x10) yields the destination room, the destination
// level and the level-warp row; the destination room's unit list (+0x74, next +0xE8) is walked for a tile (type 5 at +0) whose class (+4)
// is that level. The row is stored through ppLvlWarp whether or not a tile is found. A null room or a null destination room are fatal
// asserts in retail (not modelled: the probe never passes them).
#include <cstdint>
#include "d2common_fog_env.h"

struct D2CommonWarpEnv { void (*activateRoom)(uint32_t pDrlgRoom); };
extern "C" uint32_t D2Common_1A0D0_GetDestinationRoom(int32_t, uint32_t, uint32_t, uint32_t, const D2CommonWarpEnv*);

extern "C" uint32_t D2Common_3CD00_GetWarpTileFromRoomAndSourceLevelId(uint32_t pRoom, int32_t nSourceLevel, uint32_t ppLvlWarp, const D2CommonWarpEnv* env)
{
	using d2raw::P; using d2raw::W;
	uint32_t nDestLevel = 0, pLvlWarp = 0;
	const uint32_t pDestRoom = D2Common_1A0D0_GetDestinationRoom(nSourceLevel, W(P(pRoom), 0x10),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&nDestLevel)), static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pLvlWarp)), env);
	uint32_t pTile = 0;
	if (pDestRoom)
		for (uint32_t u = W(P(pDestRoom), 0x74); u; u = W(P(u), 0xE8))
			if (W(P(u), 0) == 5 && W(P(u), 4) == nDestLevel) { pTile = u; break; }
	W(P(ppLvlWarp), 0) = pLvlWarp;
	return pTile;
}
