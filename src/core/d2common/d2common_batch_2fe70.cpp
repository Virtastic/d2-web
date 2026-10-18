// D2Common implementation batch: +0x2FE70, +0x51ED0, +0x6B4A0, +0x3D380 translated from the pinned
// 1.13c decompiles. The vendored (1.10f) helpers are called by name where the layouts agree; callees
// that are separate still-UNMAPPED rows are seam function pointers in an environment struct passed
// last. Retail fatal paths (Fog message + _exit(-1)) are modelled as returns.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2BitManip.h"
#include "D2DataTbls.h"
#include "D2StatList.h"
#include "Units/Units.h"
#include "Units/UnitRoom.h"

// 1.13c D2Common+0x2FE70 (#10481): clear the path state that belongs to unit types that never carry
// a live path (object, item, tile clear the first path dword; every other type clears the path's
// room pointer at +0x1C). A null unit is fatal in retail (0x87D); we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_2fe70(D2UnitStrc* pUnit)
{
	if (!pUnit) return;

	const uint32_t nType = pUnit->dwUnitType;
	const bool bLeafType = nType == UNIT_OBJECT || (nType > 3 && nType < 6);

	if (bLeafType)
	{
		if (pUnit->pDynamicPath) *(uint32_t*)pUnit->pDynamicPath = 0;
	}
	else if (pUnit->pDynamicPath)
	{
		*(uint32_t*)((uint8_t*)pUnit->pDynamicPath + 0x1C) = 0;
	}
}

// 1.13c D2Common+0x51ED0 (#10483): for a unit in one of the three state-list unit types and with the
// unit flag bit at +0xC8 bit 3 set, walk the transform-state list (sgptDataTables pTransformStates,
// nTransformStates shorts). For every state whose bit is set in the unit's stat-list ex flag array,
// take that state's list, expire it, free it when it is no longer extended, then toggle the state off
// and refresh the unit. Returns 1 when the outer guard held.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_51ed0(D2UnitStrc* pUnit)
{
	if (!pUnit) return 0;
	if (((*(const uint32_t*)((const uint8_t*)pUnit + 0xC8) >> 3) & 1) == 0) return 0;

	for (int32_t i = 0; i < sgptDataTables->nTransformStates; ++i)
	{
		const int32_t nState = sgptDataTables->pTransformStates[i];
		D2StatListExStrc* pStatListEx = pUnit->pStatListEx;
		const uint32_t nType = pUnit->dwUnitType;

		if (!(nType < 2 || nType == 3)) continue;
		if (nState < 0 || nState >= sgptDataTables->nStatesTxtRecordCount) continue;
		if (!pStatListEx || (int32_t)pStatListEx->dwFlags >= 0) continue;
		if (!pStatListEx->StatFlags) continue;
		if ((pStatListEx->StatFlags[nState >> 5] & gdwBitMasks[nState & 31]) == 0) continue;

		D2StatListStrc* pStatList = D2Common_GetStateFromStatListEx_6FDB8190(pStatListEx, nState);
		if (pStatList)
		{
			D2Common_ExpireStatList_6FDB6E30(pStatList);
			if ((int32_t)pStatList->dwFlags >= 0)
			{
				D2Common_STATLIST_FreeStatListImpl_6FDB7050(pStatList);
			}
		}

		D2Common_STATES_ToggleState_6FDB8900(pUnit, nState, 0);
		UNITROOM_RefreshUnit(pUnit);
	}

	return 1;
}

namespace
{
	// Callees of 0x6B4A0 that are still UNMAPPED rows. The retail argument blobs are lost registers,
	// so the seam signatures carry the recovered arguments plus a trailing stat-list/unit.
	struct Batch2_6B4A0Env
	{
		int32_t (*collectStatList)(void* pOut, int32_t nStatId, int32_t nMax, void* pStatList); // 0x382F0
		void (*addStat)(D2UnitStrc* pUnit, int32_t nStatId, int32_t nValue);                     // 0x3A740
	};
}

// 1.13c D2Common+0x6B4A0 (#10484): apply an affix stat block to a missile unit. The two flag bits of
// the value block are folded into the missile data flags (+0x14), then thirty stat id/value pairs go
// through STATLIST_SetStat, four flag bits add stat 0x67/0x68/0x6A/0x8D, and the owning unit's stat
// 0xB4 list (read through 0x382F0, 0x80 entries) is copied onto the missile. The retail follow-up
// STATLIST_InsertStatModOrFail branch is unreachable: the unit type is 3 for the whole body.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_6b4a0(D2UnitStrc* pOwner, D2UnitStrc* pMissile,
	uint32_t* pValues, int32_t nValue12, const Batch2_6B4A0Env* pEnv)
{
	static const int32_t nStatIds[] =
	{
		0x0C, 0x15, 0x16, 0x30, 0x31, 0x13B, 0x32, 0x33, 0x34, 0x35,
		0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x146, 0x3C, 0x3D, 0x3E,
		0x3F, 0x40, 0x41, 0x42, 0x13C, 0x13D, 0x13B, 0x79, 0x7A, 0x78,
		0x19,
	};
	static_assert(sizeof(nStatIds) / sizeof(nStatIds[0]) == 31, "stat blocks 0xC + 0x15..0x1E");

	if (!pMissile || pMissile->dwUnitType != UNIT_MISSILE) return;

	uint32_t nFlags = pMissile->pMissileData ? pMissile->pMissileData->fFlags : 0;
	if (pValues[0] & 1) nFlags |= 1;
	if (pValues[0] & 4) nFlags |= 2;
	if (pMissile->pMissileData) pMissile->pMissileData->fFlags = nFlags;

	D2StatListExStrc* pStatList = pMissile->pStatListEx;

	STATLIST_SetStat(pStatList, nStatIds[0], (int32_t)nValue12, 0);
	for (int32_t i = 0; i < 30; ++i)
	{
		STATLIST_SetStat(pStatList, nStatIds[i + 1], (int32_t)pValues[i + 1], 0);
	}

	if (pEnv && pEnv->addStat)
	{
		if (pValues[0] & 0x100) pEnv->addStat(pMissile, 0x67, 1);
		if (pValues[0] & 0x200) pEnv->addStat(pMissile, 0x68, 1);
		if (pValues[0] & 0x400) pEnv->addStat(pMissile, 0x6A, 1);
		if (pValues[0] & 2) pEnv->addStat(pMissile, 0x8D, 1);
	}

	if (!pOwner || !pOwner->pStatListEx) return;
	if ((int32_t)pOwner->pStatListEx->dwFlags >= 0) return;

	int32_t nCount = 0;
	uint32_t values[256] = {};
	if (pEnv && pEnv->collectStatList)
	{
		nCount = pEnv->collectStatList(values, 0xB4, 0x80, pOwner->pStatListEx);
	}

	for (int32_t i = 0; i < nCount; ++i)
	{
		STATLIST_SetStat(pStatList, 0xB4, (int32_t)values[i * 2], 0);
	}
}

namespace
{
	// Callees of 0x3D380 that are still UNMAPPED rows: ordinal 10045 (source-tagged allocation) and
	// 0x3D250 (a stat-table reset whose arguments the decompiler loses).
	struct Batch2_3D380Env
	{
		void* (*alloc)(int32_t nSize, const char* szFile, int32_t nLine, int32_t nFlags);
		void (*reset)(void);
	};
}

// 1.13c D2Common+0x3D380 (#10485) D2FreeStats: prepend one twelve-byte record per entry of the input
// list (count byte + four-byte entries; the node takes the word at entry+4, the byte at entry+2 and
// the previous head at +8) onto the owner's list, bumping the owner's short count at +4. A null
// owner (0x10B), a null list (0x10C) or a count of 8 or more (0x112) is fatal in retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3d380(uint32_t* pOwner, const uint8_t* pList,
	const Batch2_3D380Env* pEnv)
{
	if (!pOwner) return;
	if (!pList) return;

	if (pEnv && pEnv->reset) pEnv->reset();

	const int32_t nCount = *pList;
	if (nCount >= 8) return;

	for (int32_t i = 0; i < nCount; ++i)
	{
		const uint8_t* pEntry = pList + i * 4;
		if (!pEnv || !pEnv->alloc) return;

		uint8_t* pNode = (uint8_t*)pEnv->alloc(12, nullptr, 0x5A, 0);
		if (!pNode) return;

		*(uint16_t*)pNode = *(const uint16_t*)(pEntry + 4);
		*(uint32_t*)(pNode + 4) = pEntry[2];
		*(uint32_t*)(pNode + 8) = pOwner[2];

		++*(int16_t*)((uint8_t*)pOwner + 4);
		*(uint8_t**)((uint8_t*)pOwner + 8) = pNode;
	}
}
