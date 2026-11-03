// 1.13c D2Common+0x3CD00 (#10264): warp tile for (source level, unit's room). A null unit fatals
// 0x6A. The 0x1A0D0 resolver takes (source level, unit+0x10 room, &dest level, &warp); a null result
// fatals 0x71. Otherwise the warp goes to the out pointer. Retail returns void.
//
// The retail body then walks the resolved level's record list for a type-5 entry whose id is the
// new destination level. That walk has no output, so it is not ported.
//
// This is NOT DUNGEON_GetWarpTileFromRoomAndSourceLevelId (the vendored 1.10f form, which returns
// the tile and has a different contract). Both stay in place.
#include <cstdint>
#include <emscripten.h>

#include "d2common_fatal.h"
#include "d2common_fog_env.h"

struct D2CommonWarpEnv;
extern "C" uint32_t D2Common_1A0D0_GetDestinationRoom(int32_t nSourceLevel, uint32_t pDrlgRoom,
                                                       uint32_t pDestLevel, uint32_t ppLvlWarp,
                                                       const D2CommonWarpEnv* env);

extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3cd00(uint32_t pUnit, int32_t nSourceLevel,
                                               uint32_t* pWarpOut, const D2CommonWarpEnv* env)
{
	using d2raw::P;
	using d2raw::W;
	if (!pUnit)
		d2common_fatal(0x6A);

	uint32_t nDestLevel = 0;
	uint32_t pWarp = 0;
	const uint32_t pResult = D2Common_1A0D0_GetDestinationRoom(nSourceLevel, W(P(pUnit), 0x10),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&nDestLevel)),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&pWarp)), env);
	if (!pResult)
		d2common_fatal(0x71);

	if (pWarpOut)
		*pWarpOut = pWarp;
}
