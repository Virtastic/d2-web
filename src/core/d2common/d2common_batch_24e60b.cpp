// 1.13c D2Common implementation batch: +0x302E0, +0x2E2C0, +0x2DDB0 (the UNMAPPED callee of
// +0x2E2C0, taken with it), +0x2DF80, +0x120F0, +0x121D0. Only +0x2DDB0 is kept here: the other
// five were already landed on main in src/core/d2common/d2common_batch_24e60.cpp, and defining
// them twice breaks the wasm link. +0x38B00 is the vendored
// STATLIST_GetBaseStat_6FDB6340 and is marked there (src/core/d2moo/src/D2StatList.cpp).
// Bodies translated from the pinned 1.13c decompiles. DRLG level records and their rooms are
// reached through the 1.13c 32-bit offsets (the vendored 1.10f structs reorder those fields);
// unit, path, room-coordinate and unit-find layouts agree with the vendored headers. Callees that
// are still UNMAPPED rows are env-seam function pointers (null until the environment installs
// them). Fatal paths (Fog message + _exit(-1)) are modelled as returns, as in the other d2common
// batch files.
#include <cstddef>
#include <cstdint>

#include <emscripten.h>

#include "D2DataTbls.h"
#include "D2Dungeon.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Units/UnitFinds.h"
#include "Units/Units.h"

namespace
{
	inline int32_t RdI32(const void* p, int nOffset)
	{
		return *reinterpret_cast<const int32_t*>(static_cast<const uint8_t*>(p) + nOffset);
	}
	inline void WrI32(void* p, int nOffset, uint32_t nValue)
	{
		*reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(p) + nOffset) = nValue;
	}
	inline void* RdPtr(const void* p, int nOffset)
	{
		return *reinterpret_cast<void* const*>(static_cast<const uint8_t*>(p) + nOffset);
	}

	// 1.13c D2DrlgLevelStrc: pNextLevel +0x1AC, pDrlg +0x1B4, nLevelId +0x1D0, inactivity +0x1D4;
	// pFirstRoomEx +0x10. 1.13c D2DrlgStrc: pMempool +0x478, pLevel +0x47C.
	constexpr int LEVEL_PNEXT = 0x1AC;
	constexpr int LEVEL_PDRLG = 0x1B4;
	constexpr int LEVEL_NID = 0x1D0;
	constexpr int LEVEL_INACTIVE = 0x1D4;
	constexpr int LEVEL_FIRSTROOM = 0x10;
	constexpr int DRLG_PMEMPOOL = 0x478;
	constexpr int DRLG_PLEVEL = 0x47C;

	// 1.13c D2DrlgRoomStrc: dwFlags +0x28, pDrlgRoomNext +0x24, fRoomStatus +0x44,
	// coordinate-list head +0x4C (nodes: next +0x04), second allocation +0x08, +0x2C.
	constexpr int ROOM_NEXT = 0x24;
	constexpr int ROOM_FLAGS = 0x28;
	constexpr int ROOM_STATUS = 0x44;
	constexpr int ROOM_COORDLIST = 0x4C;

	inline uint8_t* FindLevel(uint8_t* pDrlg, int32_t nLevelId)
	{
		for (uint8_t* pLevel = static_cast<uint8_t*>(RdPtr(pDrlg, DRLG_PLEVEL)); pLevel;
			pLevel = static_cast<uint8_t*>(RdPtr(pLevel, LEVEL_PNEXT)))
		{
			if (RdI32(pLevel, LEVEL_NID) == nLevelId)
			{
				return pLevel;
			}
		}
		return nullptr;
	}

	inline uint8_t* FindOrAllocLevel(uint8_t* pDrlg, int32_t nLevelId)
	{
		if (uint8_t* pLevel = FindLevel(pDrlg, nLevelId))
		{
			return pLevel;
		}
		return reinterpret_cast<uint8_t*>(
			DRLG_AllocLevel(reinterpret_cast<D2DrlgStrc*>(pDrlg), nLevelId));
	}
}

// Env seams for callees that are still-UNMAPPED rows. Signature follows the 1.13c calls (the DRLG
// pointer travels in a register the decompiler lost); a null pointer means the environment has not
// installed the implementation yet.
extern "C"
{
	// 0x44CE0: eight per-difficulty level ids for a level definition.
	int32_t* (*g_d2common_fn_44CE0)(uint32_t pDrlg, int32_t nLevelId) = nullptr;

	EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2ddb0(uint32_t pLevel);
}


// 1.13c D2Common+0x2DDB0: returns 1 when the level's room set can be discarded. First it refuses
// (0) as soon as any room in the level is active (status < 4) or flagged persistent (0x100000);
// then, for each of the eight visible level ids, it takes the visible level's room list and
// returns 0 if any room there carries a visibility bit belonging to this level and is active or
// persistent. Otherwise it releases, for every room whose flags intersect the visibility mask, the
// coordinate list at +0x4C (each node's next at +0x04) and the allocation at +0x08 (also clearing
// +0x2C), and returns 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2ddb0(uint32_t pLevelRaw)
{
	uint8_t* pLevel = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(pLevelRaw));
	if (!pLevel || !g_d2common_fn_44CE0)
	{
		return 0;
	}

	uint8_t* pDrlg = static_cast<uint8_t*>(RdPtr(pLevel, LEVEL_PDRLG));
	void* pMemPool = RdPtr(pDrlg, DRLG_PMEMPOOL);

	for (uint8_t* pRoom = static_cast<uint8_t*>(RdPtr(pLevel, LEVEL_FIRSTROOM)); pRoom;
		pRoom = static_cast<uint8_t*>(RdPtr(pRoom, ROOM_NEXT)))
	{
		if (*reinterpret_cast<uint8_t*>(pRoom + ROOM_STATUS) < 4 ||
			(static_cast<uint32_t>(RdI32(pRoom, ROOM_FLAGS)) & 0x100000) != 0)
		{
			return 0;
		}
	}

	int32_t* pVisLevelIds = g_d2common_fn_44CE0(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pDrlg)),
		RdI32(pLevel, LEVEL_NID));
	const int32_t nLevelId = RdI32(pLevel, LEVEL_NID);

	for (int i = 0; i < 8; ++i)
	{
		const int32_t nVisLevelId = pVisLevelIds[i];
		if (nVisLevelId == 0)
		{
			continue;
		}

		uint8_t* pVisLevel = FindOrAllocLevel(pDrlg, nVisLevelId);
		if (!pVisLevel)
		{
			continue;
		}

		uint8_t* pFirstRoom = static_cast<uint8_t*>(RdPtr(pVisLevel, LEVEL_FIRSTROOM));
		if (!pFirstRoom)
		{
			continue;
		}

		const int32_t* pCoords = g_d2common_fn_44CE0(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pDrlg)),
			RdI32(pVisLevel, LEVEL_NID));

		uint32_t nMask = 0;
		if (pCoords[0] == nLevelId) nMask |= 0x10;
		if (pCoords[1] == nLevelId) nMask |= 0x20;
		if (pCoords[2] == nLevelId) nMask |= 0x40;
		if (pCoords[3] == nLevelId) nMask |= 0x80;
		if (pCoords[4] == nLevelId) nMask |= 0x100;
		if (pCoords[5] == nLevelId) nMask |= 0x200;
		if (pCoords[6] == nLevelId) nMask |= 0x400;
		if (pCoords[7] == nLevelId) nMask |= 0x800;

		for (uint8_t* pRoom = pFirstRoom; pRoom; pRoom = static_cast<uint8_t*>(RdPtr(pRoom, ROOM_NEXT)))
		{
			if ((nMask & static_cast<uint32_t>(RdI32(pRoom, ROOM_FLAGS))) != 0 &&
				(*reinterpret_cast<uint8_t*>(pRoom + ROOM_STATUS) < 4 ||
					(static_cast<uint32_t>(RdI32(pRoom, ROOM_FLAGS)) & 0x100000) != 0))
			{
				return 0;
			}
		}

		for (uint8_t* pRoom = pFirstRoom; pRoom; pRoom = static_cast<uint8_t*>(RdPtr(pRoom, ROOM_NEXT)))
		{
			if ((static_cast<uint32_t>(RdI32(pRoom, ROOM_FLAGS)) & nMask) == 0)
			{
				continue;
			}

			uint8_t* pNode = static_cast<uint8_t*>(RdPtr(pRoom, ROOM_COORDLIST));
			while (pNode)
			{
				uint8_t* pNext = static_cast<uint8_t*>(RdPtr(pNode, 0x04));
				D2_FREE_POOL(pMemPool, pNode);
				pNode = pNext;
			}
			WrI32(pRoom, ROOM_COORDLIST, 0);

			if (void* pExtra = RdPtr(pRoom, 0x08))
			{
				D2_FREE_POOL(pMemPool, pExtra);
				WrI32(pRoom, 0x08, 0);
				WrI32(pRoom, 0x2C, 0);
			}
		}
	}

	return 1;
}

