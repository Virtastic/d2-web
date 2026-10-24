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
	return ITEMS_CanBeMagic(reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pItem))) ? 1 : 0;
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
	D2ActiveRoomStrc* pFound = DUNGEON_GetAdjacentRoomByTileCoordinates(
		reinterpret_cast<D2ActiveRoomStrc*>(static_cast<uintptr_t>(pRoom)), nX, nY);
	return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pFound));
}

// 1.13c D2Common+0x7C170: update the four DRLG room-allocation counters (client vs general, by flag
// bit 0 at +0x8C), then once every 5/7 timeouts walk the out-of-sight room list and ensure each room
// has its real room. Same body as the 1.10f twin DRLGACTIVATE_Update; the room visit that 1.13c
// inlines is 0x7C0E0 (DRLGACTIVATE_RoomEx_EnsureHasRoom).
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7c170(uint32_t pDrlg)
{
	DRLGACTIVATE_Update(reinterpret_cast<D2DrlgStrc*>(static_cast<uintptr_t>(pDrlg)));
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
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_82e40(int32_t nOverlayId)
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
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_374e0(uint32_t pArg)
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
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_3b470(uint32_t pUnit)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return 0;
	return W(u, 0x18);
}

// 1.13c D2Common+0x3a030 (community name D2SetAnim): merge/remove a unit's damage-related stats. The
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
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_4e0e0(uint32_t pUnit)
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
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_3b120(uint32_t pUnit, uint32_t nKey,
                                               const D2CommonBatch233B0Env* pEnv)
{
	uint8_t* u = P(pUnit);
	if (!u)
		return;
	if (pEnv && pEnv->clear_unit_flag)
		pEnv->clear_unit_flag(W(u, 0x10), nKey);
}
