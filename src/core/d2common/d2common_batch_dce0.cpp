// D2Common implementation batch b41: five UNMAPPED rows translated from the pinned 1.13c
// decompiles. Three of them have a vendored 1.10f body with the same 1.13c address recorded in the
// D2MOO headers and are reached through it; the two items.txt field getters read the row directly,
// as d2common_batch_22bc0.cpp does for nQuest.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Drlg/D2DrlgOutRoom.h"
#include "Path/Step.h"
#include "Units/Missile.h"
#include "Units/Units.h"

// 1.13c D2Common+0xDCE0 (#10223): D2Common_10232 -- the dynamic-path move step. The decompile
// fatals on a null unit or a unit that is neither player nor monster, then
// COLLISION_TrySetUnitCollisionMask(x=wPosX, y=wPosY, pDestRoom, nTargetX, nTargetY,
// dwCollisionPattern, nFootprintCollisionMask); a non-zero answer returns 0. On success a room
// change sets path flag 0x1, the game coords are recached at the target centre, the destination
// room is checked, and movement state is reset; it returns 1. The vendored D2MOO body already
// carries the 1.13c branch (Step.cpp, //1.13c 0x6FD5DCE0).
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_dce0(D2DynamicPathStrc* pPath, D2UnitStrc* pUnit,
	D2ActiveRoomStrc* pDestRoom, int32_t nTargetX, int32_t nTargetY)
{
	return D2Common_10232(pPath, pUnit, pDestRoom, nTargetX, nTargetY) ? 1 : 0;
}

// 1.13c D2Common+0x223D0: items.txt[nItemId].nTransmogrify (+0x139) != 0. An id outside the table
// (fatal 0x1EF for the null table, 0x1156 for the out-of-range id in retail) yields 0 here, the
// same answer DATATBLS_GetItemsTxtRecord gives for an out-of-range id. The 1.10f body is
// ITEMS_GetTransmogrifyFromItemId (Items.cpp), which asserts the row exists.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2common_IsTransmogrify_223d0(int32_t nItemId)
{
	static_assert(offsetof(D2ItemsTxt, nTransmogrify) == 0x139, "items.txt nTransmogrify");
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(nItemId);
	return (pRow != nullptr && pRow->nTransmogrify != 0) ? 1u : 0u;
}

// 1.13c D2Common+0x69EB0 (#10277): MISSILE_GetHomeType -- a unit of type 3 (missile) with missile
// data reports its home type (pMissileData+0x20) and home GUID (pMissileData+0x24); every other
// input (null unit, another unit type, no data block) reports home type 0 and the invalid GUID
// 0xFFFFFFFF. The vendored 1.10f body (Units/Missile.cpp) is the same function.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_69eb0(D2UnitStrc* pMissile, int32_t* pOutHomeType,
	D2UnitGUID* pOutHomeGuid)
{
	static_assert(offsetof(D2MissileDataStrc, nHomeType) == 0x20, "missile data home type");
	static_assert(offsetof(D2MissileDataStrc, dwHomeGUID) == 0x24, "missile data home GUID");
	MISSILE_GetHomeType(pMissile, pOutHomeType, pOutHomeGuid);
}

// 1.13c D2Common+0x22A20 (#10279): items.txt[nUnit->dwClassId].nDurWarning (+0x135) of an item
// unit. A null unit (fatal 0xDC4), a unit of another type (0xDC5) or a missing items row (0xDC7)
// is fatal in retail; each returns 0 here. The 1.10f body is ITEMS_GetDurWarnCount (Items.cpp).
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2common_22a20(const int32_t* pUnit)
{
	static_assert(offsetof(D2ItemsTxt, nDurWarning) == 0x135, "items.txt nDurWarning");
	if (pUnit == nullptr || pUnit[0] != UNIT_ITEM)
		return 0;
	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(pUnit[1]);
	return pRow != nullptr ? pRow->nDurWarning : 0;
}

// 1.13c D2Common+0x3C330 (#10283): for a room whose pDrlgRoom is a maze room (nType ==
// DRLGTYPE_MAZE) returns the outdoor room's flag bit 0x80, else 0. The 1.10f body is sub_6FD779F0
// (DrlgDrlgRoom.cpp); retail fatals on a null room (0x5A5), here a null pointer returns 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3c330(const D2ActiveRoomStrc* pRoom)
{
	if (pRoom == nullptr || pRoom->pDrlgRoom == nullptr)
		return 0;
	const D2DrlgRoomStrc* pDrlgRoom = pRoom->pDrlgRoom;
	if (pDrlgRoom->nType != DRLGTYPE_MAZE)
		return 0;
	return pDrlgRoom->pOutdoor->dwFlags & 0x80u;
}
