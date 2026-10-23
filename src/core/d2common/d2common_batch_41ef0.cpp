// 1.13c D2Common Stage A implementation batch: 0x41EF0, 0x23230, 0x23050, 0x4E630, 0x3E910,
// 0x318C0, 0x301D0, 0x52AE0, 0x30E20, 0x247C0, 0x51B80, 0x234A0.
// Bodies are transliterated from the pinned 1.13c decompiles in our own words. Where the 1.13c
// body is the address of a vendored D2MOO helper, the row delegates to that helper: the vendored
// body is the 1.10f twin and the 1.13c deltas were checked line by line. Retail's fatal asserts
// (_exit after a Fog message) are modelled as the function's neutral return.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Inventory.h"
#include "D2Items.h"
#include "D2Skills.h"
#include "DataTbls/AnimTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/ObjectsTbls.h"
#include "DataTbls/SkillsTbls.h"
#include "Units/Units.h"

static_assert(offsetof(D2AnimDataRecordStrc, dwFrames) == 0x08, "animdata record frames");
static_assert(offsetof(D2AnimDataRecordStrc, dwAnimSpeed) == 0x0C, "animdata record speed");
static_assert(offsetof(D2AnimDataRecordStrc, pFrameFlags) == 0x10, "animdata record frame flags");
static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80, "items.txt dwCode");
static_assert(offsetof(D2ItemsTxt, dwAlternateGfx) == 0x90, "items.txt dwAlternateGfx");
static_assert(offsetof(D2ItemsTxt, nLevel) == 0xFD, "items.txt nLevel");
static_assert(offsetof(D2SkillsTxt, nPassiveState) == 0x94, "skills.txt nPassiveState");
static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148, "skills.txt dwParam");
static_assert(sizeof(D2SkillsTxt) == 0x23C, "skills.txt row size");
static_assert(sizeof(D2ShrinesTxt) == 0xB8, "shrines.txt row size");
static_assert(offsetof(D2ItemTypesTxt, nStorePage) == 0x22, "itemtypes.txt nStorePage");
static_assert(sizeof(D2ItemTypesTxt) == 0xE4, "itemtypes.txt row size");

// Vendored, defined in src/core/d2common/d2common_batch_52660.cpp.
int32_t D2Common_22AD0_ItemMinStack(uint32_t nItemId);

// 1.13c D2Common+0x41EF0 (#10385): animation metadata for a path. On a hit the record's frame
// count (+0x08) and play speed (+0x0C) are copied out, and the first tagged frame is the first
// index below 144 whose flag byte at +0x10 is set (0 when none is). A path with no record yields
// the animdata defaults and FALSE.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_fn_d2common_41ef0(char* szPath, int32_t* pOutLength, int32_t* pOutAnimSpeed,
                             int32_t* pOutFirstFrameTagged)
{
	return DATATBLS_GetAnimDataInfo(szPath, pOutLength, pOutAnimSpeed, pOutFirstFrameTagged);
}

// 1.13c D2Common+0x23230 (#10394, D2GetObjectsBIN): the graphics code of a type-4 unit's items.txt
// row: dwAlternateGfx (+0x90) when set, else dwCode (+0x80). A null unit, another unit type or a
// missing row is fatal (0x628 / 0x629 / 0x62C) and answers 0 here.
extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t D2Common_23230_GetObjectsBIN(const int32_t* pUnit)
{
	if (pUnit == nullptr || pUnit[0] != 4)
		return 0;

	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(pUnit[1]);
	if (pRow == nullptr)
		return 0;

	return pRow->dwAlternateGfx != 0 ? pRow->dwAlternateGfx : pRow->dwCode;
}

// 1.13c D2Common+0x23050 (#10395): items.txt nLevel (+0xFD) of a type-4 unit. Null unit, another
// type or a missing row is fatal (0x697 / 0x698 / 0x69B) and answers 0 here.

uint8_t D2Common_23050_GetItemLevel(const int32_t* pUnit)
{
	if (pUnit == nullptr || pUnit[0] != 4)
		return 0;

	const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(pUnit[1]);
	if (pRow == nullptr)
		return 0;

	return pRow->nLevel;
}

// 1.13c D2Common+0x4E630 (#10400): skill param interpolation for a level. The skill id must name a
// skills.txt row and the level must be positive; both bounds are checked before the row is used.
// The value is the 0x4DC30 formula over the row's dwParam[2] (0x150) and dwParam[3] (0x154).
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t D2Common_4E630_SkillParamCurve(int32_t nLevel, int32_t nSkillId)
{
	if (sgptDataTables == nullptr)
		return 0;
	if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
		return 0;

	const D2SkillsTxt* pRow = DATATBLS_GetSkillsTxtRecord(nSkillId);
	if (pRow == nullptr || nLevel <= 0)
		return 0;

	return D2Common_11033(nLevel, static_cast<int32_t>(pRow->dwParam[2]),
	                      static_cast<int32_t>(pRow->dwParam[3]));
}

// 1.13c D2Common+0x3E910 (#10403): shrines.txt row for an id. The row stride is 0xB8 and the
// table lives at sgptDataTables+0x1094 (count +0x1098). An id outside [0, count) is fatal
// (0x15F / 0x160) and answers 0 here.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t D2Common_3E910_GetShrinesTxtRecord(int32_t nShrineId)
{
	if (sgptDataTables == nullptr || sgptDataTables->pShrinesTxt == nullptr)
		return 0;
	if (nShrineId < 0 || nShrineId >= sgptDataTables->nShrinesTxtRecordCount)
		return 0;

	return static_cast<int32_t>(
		reinterpret_cast<uintptr_t>(&sgptDataTables->pShrinesTxt[nShrineId]));
}

// 1.13c D2Common+0x318C0 (#10408): lift a unit's footprint from its room's collision grid. The
// coordinates come from the static path for objects/items/tiles (nX +0x0C, nY +0x10, both words
// at +2/+6 of the dynamic path otherwise), then the room, both footprint sizes and the unit's
// collision mask are passed to the 0x4BAB0 grid reset. This is the vendored 1.10f twin.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2common_unit_free_collision_318c0(D2UnitStrc* pUnit)
{
	UNITS_FreeCollisionPath(pUnit);
}

// 1.13c D2Common+0x301D0 (#10410): absolute difference between two units' X coordinates. Objects,
// items and tiles read the static path's nX (+0x0C); players, monsters and missiles read the
// dynamic path's wPosX (+2); a missing path counts as 0.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2common_UnitsXDistance_301d0(D2UnitStrc* pUnitA, D2UnitStrc* pUnitB)
{
	const int32_t nDelta = UNITS_GetXPosition(pUnitA) - UNITS_GetXPosition(pUnitB);
	return nDelta < 0 ? -nDelta : nDelta;
}

// 1.13c D2Common+0x52AE0 (#10418): reapply every passive skill to a unit. The passive list
// (sgptDataTables+0xBB4, count +0xBB0) yields short skill ids; each must name a skills.txt row
// with nPassiveState > 0 (+0x94). The unit's highest-level skill of that id must exist, the state
// must be in range (0xC4) and set on the unit (0x37DB0 bit test over types 0/1/3), and only then
// is the skill refreshed (0x52480). This is the vendored 1.10f twin.
extern "C" EMSCRIPTEN_KEEPALIVE
void D2Common_52AE0_RefreshPassives(D2UnitStrc* pUnit)
{
	SKILLS_RefreshPassiveSkills(pUnit);
}

// 1.13c D2Common+0x30E20 (#10420): make pTargetUnit the path target of a player or monster. The
// dynamic path stores the unit at +0x58 and, when it is non-null, its type at +0x5C and its id at
// +0x60. A null unit is fatal 0x8DC and another unit type fatal 0x8DE; both are returns here.
extern "C" EMSCRIPTEN_KEEPALIVE
void D2Common_30E20_SetTargetUnit(D2UnitStrc* pUnit, D2UnitStrc* pTargetUnit)
{
	if (pUnit == nullptr)
		return;
	if (pUnit->dwUnitType != 0 && pUnit->dwUnitType != 1)
		return;

	UNITS_SetTargetUnitForPlayerOrMonster(pUnit, pTargetUnit);
}

// 1.13c D2Common+0x247C0 (#10432): minimum stack of the items.txt row of a type-4 unit. Null unit
// is fatal 0xD94 and another unit type fatal 0xD95; both are returns here.
// ponytail: no export, plain C++
int32_t D2Common_247C0_UnitItemMinStack(const int32_t* pUnit)
{
	if (pUnit == nullptr || pUnit[0] != 4)
		return 0;

	return D2Common_22AD0_ItemMinStack(static_cast<uint32_t>(pUnit[1]));
}

// The skill-description evaluator is the Fog ordinal 10253 (RVA 0x9270), which walks the byte code
// with the two fixed callback tables of this call site. Those tables are retail data, so the
// evaluator arrives through a seam; a null seam leaves the call without an answer.
struct D2Common_51B80_Env
{
	int32_t (*evaluate)(uint32_t pExpression, int32_t nExpressionSize, const uint32_t* pUser);
};

// 1.13c D2Common+0x51B80 (#10435): evaluate the skill-description byte code from nIndex. The code
// block is the pinned skill-desc block (sgptDataTables+0x4C, size +0x50) and the three user dwords
// are the caller's first, third and fourth arguments.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t D2Common_SkillDescEval(uint32_t nContext, uint32_t nIndex, uint32_t nArg1, uint32_t nArg2,
                             const D2Common_51B80_Env* pEnv)
{
	if (sgptDataTables == nullptr || sgptDataTables->pSkillDescCode == nullptr)
		return 0;
	if (nIndex >= sgptDataTables->nSkillDescCodeSize)
		return 0;
	if (pEnv == nullptr || pEnv->evaluate == nullptr)
		return 0;

	const uint32_t user[3] = { nContext, nArg1, nArg2 };
	const uint32_t nExpression = static_cast<uint32_t>(
		reinterpret_cast<uintptr_t>(sgptDataTables->pSkillDescCode)) + nIndex;
	return pEnv->evaluate(nExpression,
	                      static_cast<int32_t>(sgptDataTables->nSkillDescCodeSize - nIndex), user);
}

// 1.13c D2Common+0x234a0 (#11075): the store page (itemtypes.txt nStorePage, byte +0x22) of a
// type-4 unit's item type. A null unit, another type, an item type outside the row count or a
// -1/absent row answers -1.
extern "C" EMSCRIPTEN_KEEPALIVE
int8_t d2_fn_d2common_234a0(D2UnitStrc* pItem)
{
	if (pItem == nullptr || pItem->dwUnitType != 4)
		return -1;

	const int32_t nItemType = static_cast<int32_t>(ITEMS_GetItemType(pItem));
	if (nItemType < 0 || sgptDataTables == nullptr || sgptDataTables->pItemTypesTxt == nullptr)
		return -1;
	if (nItemType >= sgptDataTables->nItemTypesTxtRecordCount)
		return -1;

	const int8_t nStorePage =
		static_cast<int8_t>(sgptDataTables->pItemTypesTxt[nItemType].nStorePage);
	return nStorePage != -1 ? nStorePage : -1;
}
