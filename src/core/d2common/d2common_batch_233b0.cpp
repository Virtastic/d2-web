#include <cstring>
// 1.13c D2Common implementation batch (rows 0x233B0, 0x24C30, 0x3C120, 0x7C170, 0x3B550, 0x82E40,
// 0x12D20, 0x374E0, 0x3B470, 0x3A030, 0x4E0E0, 0x3B120).
// Six of the bodies are the same code as a vendored D2MOO 1.10f twin (same branches, same fields), so
// they delegate; the rest are written from the cached 1.13c decompiles. Callees whose bodies are not
// recovered are env-seam function pointers after the retail arguments; a null seam contributes nothing
// and the body never faults on it. Fatal paths ending in _exit(-1) are modelled as returns, as in the
// rest of src/core/d2common.
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Dungeon.h"
#include "D2Items.h"
#include "D2StatList.h"
#include "Drlg/D2DrlgActivate.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline uint32_t R(const uint8_t* b, int32_t o) { return *reinterpret_cast<const uint32_t*>(b + o); }
}

// Seams for the two bodies that are not a vendored twin: the logging subsystem (0x12C90 opens a log
// file, 0x12AA0 closes it, 0x6FDF1474 remembers the last file name) and 0x19810, which walks an owner
// blob's chains and clears/sets unit flag 8 for a key.
struct D2CommonBatch233B0Env
{
	void (*log_open)(const char* szFile);                         // 0x12C90
	void (*log_close)();                                          // 0x12AA0
	uint32_t* pLogFileName;                                       // static 0x6FDF1474
	void (*clear_unit_flag)(uint32_t pOwnerBlob, uint32_t nKey);  // 0x19810
};

// 1.13c D2Common+0x233B0 (ITEMS_CanBeMagic_1_13c): whether an item unit's ItemTypesTxt row (reachable through 0x230F0 and the
// item's class id) has nMagic set. The 1.10f twin is ITEMS_CanBeMagic; a null or non-item unit, an
// out-of-range type index and a null row all yield 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t ITEMS_CanBeMagic_1_13c(uint32_t pItem)
{
	// Follows the decompile: no assert on a null items.txt row; every miss is 0.
	const D2UnitStrc* pUnit = reinterpret_cast<const D2UnitStrc*>(static_cast<uintptr_t>(pItem));
	if (!pUnit || pUnit->dwUnitType != UNIT_ITEM)
		return 0;
	const D2ItemsTxt* pItemsTxtRecord = DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
	if (!pItemsTxtRecord)
		return 0;
	const int32_t nType = pItemsTxtRecord->wType[0];
	if (nType < 0 || nType >= sgptDataTables->nItemTypesTxtRecordCount || !sgptDataTables->pItemTypesTxt)
		return 0;
	return sgptDataTables->pItemTypesTxt[nType].nMagic;
}

// 1.13c D2Common+0x24C30 (community name D2ItemSetFlags, but the body reads the item type's
// nThrowable byte at +0x10): the 1.10f twin is ITEMS_CheckIfThrowable.
extern "C"
EMSCRIPTEN_KEEPALIVE uint8_t D2Common_ItemIsThrowable_24C30(uint32_t pItem)
{
	return static_cast<uint8_t>(ITEMS_CheckIfThrowable(
		reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pItem))));
}

// 1.13c D2Common+0x3C120: first room in the room's own list whose tile rectangle contains (nX, nY).
// ppRoomList is +0x00 and nNumRooms +0x24; a null list entry is a retail assert (0x1B3). The 1.10f
// twin is DUNGEON_GetAdjacentRoomByTileCoordinates; 0x3B910 is 1.13c's AreTileCoordinatesInsideRoom.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2Common_3C120_GetAdjacentRoomByTile(uint32_t pRoom, int32_t nX, int32_t nY)
{
	// Retail 32-bit layout: list pointer +0x00, count +0x24; each listed room's tile rect is
	// x +0x5C, y +0x60, width +0x64, height +0x68 (0x3B910's test).
	if (!pRoom)
		return 0;
	const uint8_t* r = P(pRoom);
	const uint8_t* list = P(R(r, 0x00));
	const int32_t nNum = static_cast<int32_t>(R(r, 0x24));
	for (int32_t i = 0; i < nNum; ++i)
	{
		const uint32_t pTemp = R(list, i * 4);
		if (!pTemp)
			return 0;
		const uint8_t* t = P(pTemp);
		const int32_t x = static_cast<int32_t>(R(t, 0x5C)), y = static_cast<int32_t>(R(t, 0x60));
		const int32_t w = static_cast<int32_t>(R(t, 0x64)), h = static_cast<int32_t>(R(t, 0x68));
		if (nX >= x && nX < x + w && nY >= y && nY < y + h)
			return pTemp;
	}
	return 0;
}

// 1.13c D2Common+0x7C170 (DRLGACTIVATE_Update, stack arg = the lists struct, ret 4): mirror the +8 / +0x468
// pair into the four DAT_6FDEFB84.. globals when lists+0x8C bit 0 is set, then once every 5 or 7 timeouts
// (+0x45C) walk the status-2 ring from +0x460 and run 0x7C0E0 on each idle room. Retail 32-bit layout:
// pDrlg and every link are retail tokens; the globals are passed in as G[4] (84, 88, 8C, 90).
void sub_7C0E0(void* unit);
extern "C"
void D2Common_7C170_ActivateUpdate(uint32_t pDrlg, uint32_t* G)
{
	if (pDrlg == 0)
		return;
	auto rd32 = [](uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)) + o, 4); return v; };
	auto wr32 = [](uint32_t a, uint32_t o, uint32_t v) { std::memcpy(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)) + o, &v, 4); };
	auto rd8 = [](uint32_t a, uint32_t o) { return *(reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)) + o); };
	auto wr8 = [](uint32_t a, uint32_t o, uint8_t v) { *(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)) + o) = v; };
	uint32_t u4 = rd32(pDrlg, 0x468);
	uint32_t u3 = rd32(pDrlg, 8);
	if (rd32(pDrlg, 0x8C) & 1)
	{
		G[0] = rd32(pDrlg, 8);
		G[1] = rd32(pDrlg, 0x468);
		u4 = G[2];
		u3 = G[3];
	}
	G[3] = u3;
	G[2] = u4;
	if (rd8(pDrlg, 0x98) < 2)
	{
		wr8(pDrlg, 0x45C, static_cast<uint8_t>(rd8(pDrlg, 0x45C) - 1));
		if (rd8(pDrlg, 0x45C) == 0)
		{
			wr8(pDrlg, 0x45C, static_cast<uint8_t>((~(static_cast<uint8_t>(rd32(pDrlg, 0x8C)) << 1) & 2) | 5));
			const uint32_t cur = rd32(pDrlg, 0x460);
			if (cur == 0 || rd8(cur, 0x44) != 2)
				wr32(pDrlg, 0x460, rd32(pDrlg, 0x294));
			const uint32_t start = rd32(pDrlg, 0x460);
			uint32_t room = start;
			if (start != 0)
			{
				do
				{
					if ((rd32(room, 0x28) & 0x100000) == 0 && room != pDrlg + 0x278 && rd32(room, 0x30) == 0)
						sub_7C0E0(reinterpret_cast<void*>(static_cast<uintptr_t>(room)));
					room = rd32(room, 0x1C);
				} while (room != start && rd8(pDrlg, 0x98) == 0);
				wr32(pDrlg, 0x460, room);
				wr8(pDrlg, 0x98, 0);
			}
		}
		return;
	}
	wr8(pDrlg, 0x98, 0);
}

// 1.13c D2Common+0x3B550: dword at unit+0x10. Null unit is retail fatal 0x31E.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3b550(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	return W(u, 0x10);
}

// 1.13c D2Common+0x82E40: OverlayTxt[nOverlayId].dwOffsetY (row stride 0x84, field at 0x58); an
// out-of-range id or a null row yields 0. The 1.10f twin is DATATBLS_GetOffsetYFromOverlayTxt.
// Not an export: the FN probe src/core/fn/d2common_82e40.cpp owns d2_fn_d2common_82e40 (r[] ABI).
extern "C"
uint32_t D2Common_82E40_OverlayOffsetY(int32_t nOverlayId)
{
	return DATATBLS_GetOffsetYFromOverlayTxt(nOverlayId);
}

// 1.13c D2Common+0x12D20: remember the last log file name and open it, or close logging when the name
// is null. The two calls are 0x12C90 (open with mode "wt") and 0x12AA0 (close), both env seams.
extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_12D20_SetLogFile(const char* szFile, const D2CommonBatch233B0Env* pEnv)
{
	if (pEnv && pEnv->pLogFileName)
		*pEnv->pLogFileName = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(szFile));

	if (szFile)
	{
		if (pEnv && pEnv->log_open)
			pEnv->log_open(szFile);
	}
	else if (pEnv && pEnv->log_close)
	{
		pEnv->log_close();
	}
}

// 1.13c D2Common+0x374e0: dword at +0x0C of the object pointed to by +0x04; a null +0x04 yields 0.
// Null param is retail fatal 0x547.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_374e0_batch(uint32_t pArg)
{
	uint8_t* a = P(pArg);
	if (!a)
		return 0;
	const uint32_t pInner = W(a, 0x04);
	if (!pInner)
		return 0;
	return R(P(pInner), 0x0C);
}

// 1.13c D2Common+0x3b470: dword at unit+0x18. Null unit is retail fatal 0x3A6.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3b470_batch(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	return W(u, 0x18);
}

// D2Common 0x3a030 (community name D2SetAnim): merge/remove a unit's damage-related stats. The
// body is the subtract direction of the 1.10f twin shared by D2Common_11274 (0x3A100) and
// D2Common_11275, including the stat-list merge, the extended-list requirement and the -value removal
// for stats whose ItemStatCostTxt flags carry gdwBitMasks[DAMAGERELATED]. A null target or source unit
// yields 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3a030(uint32_t pTarget, uint32_t pUnit)
{
	return D2Common_11275(reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pTarget)),
	                      reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit))) ? 1 : 0;
}

// 1.13c D2Common+0x4e0e0: dword at unit+0x14. Null unit is retail fatal 0xA36.
extern "C"
uint32_t d2_fn_d2common_4e0e0_batch(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	return W(u, 0x14);
}

// 1.13c D2Common+0x3b120: call 0x19810 with the owner blob at +0x10 and the second argument as the
// key. The decompiler drops both the second stack argument and the register argument; retail's
// `mov eax,[esp+8]; mov ecx,[ecx+0x10]; call` is reproduced here. Null param is retail fatal 0x5CF.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3b120_batch(uint32_t pUnit, uint32_t nKey,
                                               const D2CommonBatch233B0Env* pEnv)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return;
	if (pEnv && pEnv->clear_unit_flag)
		pEnv->clear_unit_flag(W(u, 0x10), nKey);
}
