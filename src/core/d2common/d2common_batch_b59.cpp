// 1.13c D2Common implementation batch: +0x3CDF0, +0x3CEE0.
// Bodies follow the cached 1.13c decompile. Callees that are still separate UNMAPPED rows arrive
// through an env struct passed after the retail arguments (a null env leaves the call inert);
// retail's fatal paths (Fog message + _exit(-1)) are modelled as returns, as in the other
// d2common batch files.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

#include "D2Skills.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/SkillsTbls.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t R(const uint8_t* b, int o) { return *reinterpret_cast<const uint32_t*>(b + o); }
}

// Seams for this row's UNMAPPED callees, in retail argument order.
// 0x1A130: (unit context, level id, request type, out x, out y) -> level/field record.
// The y output travels in EAX in retail; the seam takes it as the last argument.
// 0x4D140: (record, in/out {x,y}, match block, flags, radius, step flags, allow untouched,
//           max radius, step) -> chosen record or NULL.
// 0x1330: (record, subtile x, subtile y) -> room containing the point, or NULL.
struct D2Common3CDF0Env
{
	int32_t* (*resolve_room)(uint32_t pContext, int32_t nLevelId, int32_t nType,
	                         int32_t* pOutX, int32_t* pOutY);					//0x00 0x1A130
	int32_t* (*find_field_record)(uint32_t pContext, int32_t* pCoords, uint32_t* pMatchBlock,
	                              int32_t nFlags, uint32_t nRadius, uint32_t nStepFlags,
	                              int32_t nAllowUntouched, uint32_t nMaxRadius,
	                              int32_t nStep);								//0x04 0x4D140
	int32_t* (*room_at)(int32_t* pRoom, int32_t nX, int32_t nY);				//0x08 0x1330
};

// 1.13c D2Common+0x3CDF0 (#10816): resolve the level/field record for (unit+0x48, arg2, arg3),
// then expand a square of radius 0x32 step 1 around the record's coordinates scaled by 5 and
// biased by 3, accept the first record that touches the requested flags (0x1C09) and return the
// room holding the chosen cell, writing the chosen coordinate back through arg4/arg5.
// The record's coordinates arrive through the 0x1A130 seam's two outputs.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t* d2_fn_d2common_3cdf0(uint32_t pUnit, int32_t nArg2, int32_t nArg3,
                                                   int32_t* pOutX, int32_t* pOutY, int32_t nArg6,
                                                   const D2Common3CDF0Env* pEnv)
{
	if (!pUnit)
		return nullptr;
	if (!pEnv || !pEnv->resolve_room || !pEnv->find_field_record || !pEnv->room_at)
		return nullptr;

	const uint32_t nContext = R(P(pUnit), 0x48);

	int32_t nX = 0;
	int32_t nY = 0;
	int32_t* pRecord = pEnv->resolve_room(nContext, nArg2, nArg3, &nX, &nY);
	if (!pRecord)
		return nullptr;

	int32_t coords[2] = { nX * 5 + 3, nY * 5 + 3 };
	if (!pEnv->find_field_record(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pRecord)), coords, nullptr, nArg6,
	                             0x1C09, 0x1C09, 0, 0x32, 1))
		return nullptr;

	if (pOutX)
		*pOutX = coords[0];
	if (pOutY)
		*pOutY = coords[1];

	return pEnv->room_at(pRecord, coords[0], coords[1]);
}

// 1.13c D2Common+0x3CEE0 (#10942): null-check the unit and forward (unit+0x48, arg2, arg3, out x,
// out y) to the 0x1A130 record resolver; the resolver's return value is the caller's result.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t* d2_fn_d2common_3cee0(uint32_t pUnit, int32_t nArg2, int32_t nArg3,
                                                   int32_t* pOutX, int32_t* pOutY,
                                                   const D2Common3CDF0Env* pEnv)
{
	if (!pUnit)
		return nullptr;
	if (!pEnv || !pEnv->resolve_room)
		return nullptr;
	return pEnv->resolve_room(R(P(pUnit), 0x48), nArg2, nArg3, pOutX, pOutY);
}

// 1.13c D2Common+0x39B30 (#10810): count of nItemId in the unit's inventory grid. The grid is at
// unit+0x5C; only a grid whose +0x10 is negative (not yet materialised) is consulted, and the
// item id travels in the high 16 bits of the callee's word.
struct D2Common39B30Env
{
	int32_t (*has_item)(uint32_t pInventory, int32_t nItemId16);					//0x00 0x39530
};

// 1.13c D2Common+0x39b30
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_39b30(uint32_t pUnit, int32_t nItemId,
                                                  const D2Common39B30Env* pEnv)
{
	if (!pUnit)
		return 0;
	const uint32_t pInventory = R(P(pUnit), 0x5C);
	if (!pInventory || static_cast<int32_t>(R(P(pInventory), 0x10)) >= 0)
		return 0;
	if (!pEnv || !pEnv->has_item)
		return 0;
	return pEnv->has_item(pInventory, static_cast<int32_t>(static_cast<uint32_t>(nItemId) << 16));
}

// 1.13c D2Common+0x4e5e0 (#10832): skills.txt row nSkillId -> its combined value at nLevel, the
// same interpolation as D2Common_11033 over dwParam[4] (0x158) and dwParam[5] (0x15C). A bad id
// or a non-positive level yields 0.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_4e5e0(int32_t nLevel, int32_t nSkillId)
{
	static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "skills.txt dwParam");

	const D2SkillsTxt* pTxt = DATATBLS_GetSkillsTxtRecord(nSkillId);
	if (!pTxt || nLevel <= 0)
		return 0;
	return D2Common_11033(nLevel, pTxt->dwParam[4], pTxt->dwParam[5]);
}

// 1.13c D2Common+0x2f8a0 (#10835): items.txt nBelt (+0x130) for an item unit's class id
// (unit type 4, class id at +0x04). Retail treats a non-item unit or a bad class id as a fatal
// error; those paths answer 0 here.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t d2_fn_d2common_2f8a0(uint32_t pUnit)
{
	static_assert(offsetof(D2ItemsTxt, nBelt) == 0x130, "items.txt nBelt");
	static_assert(sizeof(D2ItemsTxt) == 0x1A8, "items.txt record stride");

	if (!pUnit || R(P(pUnit), 0x00) != 4)
		return 0;
	const D2ItemsTxt* pTxt = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(R(P(pUnit), 0x04)));
	return pTxt ? pTxt->nBelt : 0;
}
