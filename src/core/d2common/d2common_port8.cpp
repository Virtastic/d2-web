// 1.13c D2Common port batch 8b: +0x0F090 +0x12A20 +0x16C90 +0x19C90 +0x38C80 +0x39490 +0x5A6C0 +0x67700.
// Transcribed from the 1.13c decompile (no disassembly was available when this was written, so the
// register assignment of hidden callee arguments is the decompiler's; each is flagged in the spec).
// Retail-layout blobs are addressed by 32-bit offsets (P/W below), as in the sibling batch files;
// callees that are not vendored are weak seams, null until installed.
#include <cstdint>
#include <cstdio>
#include <cstddef>
#include <initializer_list>
#if __has_include(<emscripten.h>)
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include "Drlg/D2DrlgMaze.h"
#include "Drlg/D2DrlgDrlg.h"

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t W(uint32_t a, uint32_t o) { return *reinterpret_cast<const uint32_t*>(P(a) + o); }
	inline void W(uint32_t a, uint32_t o, uint32_t v) { *reinterpret_cast<uint32_t*>(P(a) + o) = v; }
}

extern "C" {
// seams for callees that have no vendored body (arguments as the decompile passes them)
__attribute__((weak)) void d2_port8_GetLocalTime(uint16_t* pSystemTime8);                     // kernel32 GetLocalTime (SYSTEMTIME, 16 bytes)
__attribute__((weak)) uint32_t d2_port8_7D9B0_AllocLevel(uint32_t pDrlg, int32_t nLevelId);   // D2Common+0x2D9B0
__attribute__((weak)) int32_t d2_port8_7D470_Direction(uint32_t a, uint32_t b);               // D2Common+0x2D470
__attribute__((weak)) void d2_port8_44F20_AddOrth(uint32_t pHead, uint32_t pLevel, int32_t nDir, int32_t bPreset); // +0x44F20
__attribute__((weak)) uint32_t d2_port8_44CE0_LinkArray(uint32_t pDrlg, int32_t nLevelId);    // +0x44CE0 -> int32_t[8]
__attribute__((weak)) void d2_port8_7E360_BuildLevel(uint32_t pLevel);                        // +0x2E360
__attribute__((weak)) void d2_port8_19950_Touch(uint32_t pDrlg, int32_t nLevelId);            // +0x19950, result unused here
__attribute__((weak)) void d2_port8_68400_Room(uint32_t pRoom);                               // +0x68400 (retail 6FDB8400)
__attribute__((weak)) void d2_port8_BF70_Room(uint32_t pRoom);                                // +0x0BF70
__attribute__((weak)) void d2_port8_7C0E0_Room(uint32_t pRoom);                               // +0x7C0E0 (retail 6FDCC0E0)
__attribute__((weak)) void d2_port8_gen0(void);                                               // +0x17AE0 .. +0x17930, act generators
__attribute__((weak)) void d2_port8_gen1(void);
__attribute__((weak)) void d2_port8_gen2(void);
__attribute__((weak)) void d2_port8_gen3(void);
__attribute__((weak)) void d2_port8_gen4(void);
__attribute__((weak)) int32_t d2_sub_38B00_StatValue(uint32_t pList, int32_t nStatId, uint32_t pRec); // +0x38B00 (D2Common_38B00_StatValue)
__attribute__((weak)) int32_t d2_sub_38A80_StatMax(uint32_t pList, int32_t nStatId, uint32_t pRec);   // +0x38A80
__attribute__((weak)) uint32_t d2_port8_5EF40_CompileTxt(uint32_t hArchive, uint32_t szName, const uint32_t* pTbl, uint32_t pCountOut, uint32_t dwSize);
__attribute__((weak)) uint32_t d2_port8_4CE90_Collision(uint32_t pRoom, int32_t x, int32_t y, uint32_t nPattern, uint32_t nMask);
uint32_t d2_fn_d2common_67300(const uint8_t* p1, uint32_t p2, uint32_t p3);  // batch_67300.cpp
// retail addresses of the Name-column field callback / strings in the 1.13c image (DAT_6fddab48, LAB_6fdae350, DAT_6fdda3e0)
__attribute__((weak)) extern const char d2_port8_dat_ddab48[];
__attribute__((weak)) extern const char d2_port8_dat_dda3e0[];
__attribute__((weak)) extern const char d2_port8_lab_dae350[];
}

// 1.13c D2Common+0x0F090 (ECX = parent room, EAX = direction, EDI = merge flag).
// Allocate and link a maze room next to the parent (0x0F000); if that succeeded, re-pick the preset of the
// parent when merging and always of the new room (0x0E490 with reset 1). Returns the new room or null.
extern "C" EMSCRIPTEN_KEEPALIVE D2DrlgRoomStrc* D2Common_F090_PlaceAdjacentRoomAndPickPreset(D2DrlgRoomStrc* pParent, int32_t nDirection, int32_t bMerge)
{
	D2DrlgRoomStrc* pNew = DRLGMAZE_AllocLinkedMazeRoom(pParent, nDirection, bMerge);
	if (pNew)
	{
		if (bMerge)
			DRLGMAZE_PickRoomPreset(pParent, 1);
		DRLGMAZE_PickRoomPreset(pNew, 1);
	}
	return pNew;
}

// 1.13c D2Common+0x12A20 (arg: output buffer). "HH:MM:SS.mmm<TAB>", 13 chars + NUL, from the local time.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_12A20_FormatTimestampPrefix(char* szOut)
{
	uint16_t st[8] = {};   // wYear wMonth wDayOfWeek wDay wHour wMinute wSecond wMilliseconds
	if (d2_port8_GetLocalTime)
		d2_port8_GetLocalTime(st);
	snprintf(szOut, 14, "%02d:%02d:%02d.%03d\t", (unsigned)st[4], (unsigned)st[5], (unsigned)st[6], (unsigned)st[7]);
}

namespace
{
	// level record list of a DRLG (retail layout): head +0x47C, next +0x1AC, id +0x1D0; else allocate (0x2D9B0)
	uint32_t FindOrAllocLevel(uint32_t pDrlg, int32_t nLevelId)
	{
		for (uint32_t p = W(pDrlg, 0x47C); p; p = W(p, 0x1AC))
			if (static_cast<int32_t>(W(p, 0x1D0)) == nLevelId)
				return p;
		return d2_port8_7D9B0_AllocLevel(pDrlg, nLevelId);
	}
	// first room of a level (head +0x10, next +0x24, flags +0x28) whose flags carry bit nBit; 0 if none
	uint32_t RoomWithFlag(uint32_t pLevel, uint32_t nBit)
	{
		for (uint32_t r = W(pLevel, 0x10); r; r = W(r, 0x24))
			if (W(r, 0x28) & (1u << (nBit & 0x1F)))
				return r;
		return 0;
	}
	// n-th (0-based) occurrence of nId in the eight-entry link array; -1 if absent
	int32_t NthLinkSlot(uint32_t pArray, int32_t nId, int32_t n)
	{
		int32_t seen = 0;
		for (uint32_t i = 0; i < 8; ++i)
			if (static_cast<int32_t>(W(pArray, i * 4)) == nId)
			{
				if (seen == n) return static_cast<int32_t>(i);
				++seen;
			}
		return -1;
	}
}

// 1.13c D2Common+0x16C90 (EAX = level record, EDX = target level id; ECX unused).
// Find (or allocate) the level with that id in the DRLG at level+0x1B4, then add an orthogonal link on it:
// direction from 0x2D470 over (found+0x00, found+0x1C), node added with 0x44F20.
// UNVERIFIED: which register carries head/level/preset into 0x44F20 (decompile hides them); head = found
// level slot, level = EAX, preset = 0 are the decompile-consistent reading.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_16C90_LinkLevelRecordById(uint32_t pLevel, int32_t nLevelId)
{
	const uint32_t pDrlg = W(pLevel, 0x1B4);
	const uint32_t pFound = FindOrAllocLevel(pDrlg, nLevelId);
	const int32_t nDir = d2_port8_7D470_Direction(W(pFound, 0), pFound + 0x1C);
	d2_port8_44F20_AddOrth(pFound, pLevel, nDir, 0);
}

// 1.13c D2Common+0x19C90 (AL = act selector 0..4, stack: pDrlg, idA, idB).
// Run the act generator for the selector, make sure both levels have rooms (0x2E360), then for up to eight
// link-slot pairs (the n-th idB in A's link array, the n-th idA in B's) ready the room behind each slot:
// 0x68400 unless flag 0x1000000, 0x0BF70 when 0x2000000 clear and room type (+0x48) is 2, 0x7C0E0 unless
// flag 0x100000; then flag 0x4000000 is set unless 0x40000 or 0x4000000 is already set. Stops at the first
// pair missing a slot. A null room (retail would fault) ends the walk here.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_19C90_LinkLevelPairByIndex(uint8_t nSelector, uint32_t pDrlg, int32_t nIdA, int32_t nIdB)
{
	switch (nSelector)
	{
	case 0: if (d2_port8_gen0) d2_port8_gen0(); break;   // 0x17AE0
	case 1: if (d2_port8_gen1) d2_port8_gen1(); break;   // 0x17AA0
	case 2: if (d2_port8_gen2) d2_port8_gen2(); break;   // 0x17A00
	case 3: if (d2_port8_gen3) d2_port8_gen3(); break;   // 0x17990
	case 4: if (d2_port8_gen4) d2_port8_gen4(); break;   // 0x17930
	}
	const uint32_t pA = FindOrAllocLevel(pDrlg, nIdA);
	if (W(pA, 0x10) == 0) d2_port8_7E360_BuildLevel(pA);
	const uint32_t pB = FindOrAllocLevel(pDrlg, nIdB);
	if (W(pB, 0x10) == 0) d2_port8_7E360_BuildLevel(pB);

	for (int32_t n = 0; n < 8; ++n)
	{
		const int32_t sA = NthLinkSlot(d2_port8_44CE0_LinkArray(pDrlg, nIdA), nIdB, n);
		const int32_t sB = NthLinkSlot(d2_port8_44CE0_LinkArray(pDrlg, nIdB), nIdA, n);
		if (sA == -1 || sB == -1)
			return;
		d2_port8_19950_Touch(pDrlg, static_cast<int32_t>(W(pA, 0x1D0)));
		d2_port8_19950_Touch(pDrlg, static_cast<int32_t>(W(pB, 0x1D0)));
		const uint32_t rA = RoomWithFlag(pA, static_cast<uint32_t>(sA) + 4);
		const uint32_t rB = RoomWithFlag(pB, static_cast<uint32_t>(sB) + 4);
		if (!rA || !rB)
			return;
		for (const uint32_t r : { rA, rB })
		{
			if (!(W(r, 0x28) & 0x1000000)) d2_port8_68400_Room(r);
			if (!(W(r, 0x28) & 0x2000000) && W(r, 0x48) == 2) d2_port8_BF70_Room(r);
			if (!(W(r, 0x28) & 0x100000)) d2_port8_7C0E0_Room(r);
		}
		for (const uint32_t r : { rA, rB })
		{
			const uint32_t f = W(r, 0x28);
			if (!(f & 0x40000) && !(f & 0x4000000))
				W(r, 0x28, f | 0x4000000);
		}
	}
}

namespace
{
	// ItemStatCost record of stat nStatCostId (table +0xBCC, count +0xBD4, stride 0x144), 0 when out of range
	uint32_t StatCostRecord(uint32_t pDataTables, int32_t nStatCostId)
	{
		if (nStatCostId < 0 || nStatCostId >= static_cast<int32_t>(W(pDataTables, 0xBD4)))
			return 0;
		return W(pDataTables, 0xBCC) + static_cast<uint32_t>(nStatCostId) * 0x144u;
	}
}

// 1.13c D2Common+0x38C80 (EAX = stat-cost id, ECX = stat list; EBX/EDI pass through to 0x38B00).
// Null list or id outside [0, count) -> 0; else 0x38B00(list, packed id, record).
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_38C80_StatValueById(uint32_t pDataTables, int32_t nStatCostId, uint32_t pList, int32_t nPackedStatId)
{
	const uint32_t rec = pList ? StatCostRecord(pDataTables, nStatCostId) : 0;
	return rec ? d2_sub_38B00_StatValue(pList, nPackedStatId, rec) : 0;
}

// 1.13c D2Common+0x39490: same gates, calls 0x38A80 (the clamped/maximum variant) instead.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t D2Common_39490_StatMaxById(uint32_t pDataTables, int32_t nStatCostId, uint32_t pList, int32_t nPackedStatId)
{
	const uint32_t rec = pList ? StatCostRecord(pDataTables, nStatCostId) : 0;
	return rec ? d2_sub_38A80_StatMax(pList, nPackedStatId, rec) : 0;
}

// 1.13c D2Common+0x5A6C0 (ECX = record size, EDX = txt name, stack = archive, ESI = slot 0..3).
// Compile a one-column txt (field table: one entry, type 0x16, offset 0, callback LAB_6fdae350, terminated by
// a zero entry) into slot nSlot: record array -> slot data (DAT_6fdf0b64[slot]), count -> DAT_6fdf0b74[slot].
// Out-of-range slot does nothing. Table is built in the retail 32-bit layout (10 words).
static uint32_t g_port8_slotData[4];    // DAT_6fdf0b64
static int32_t g_port8_slotCount[4];    // DAT_6fdf0b74
extern "C" EMSCRIPTEN_KEEPALIVE void D2Common_5A6C0_LoadTxtIntoSlot(uint32_t dwRecordSize, uint32_t szName, uint32_t hArchive, int32_t nSlot)
{
	const uint32_t tbl[10] = {
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(d2_port8_dat_ddab48)), 0x16, 0, 0,
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(d2_port8_lab_dae350)),
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(d2_port8_dat_dda3e0)), 0, 0, 0, 0 };
	if (nSlot < 0 || nSlot >= 4 || !d2_port8_5EF40_CompileTxt)
		return;
	g_port8_slotData[nSlot] = d2_port8_5EF40_CompileTxt(hArchive, szName, tbl,
		static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&g_port8_slotCount[nSlot])), dwRecordSize);
}

// 1.13c D2Common+0x67700 (EAX = route holder, whose +0x30 points at the path record).
// Path record: +0x1C room, +0x28 waypoint count, +0x48 collision pattern, +0x50 mask, +0x9C packed waypoints
// (x low 16, y high 16); holder +0x00 = packed start cell. Each hop: both axes within 1 cell -> collision
// check 0x4CE90 on the waypoint cell (any collision fails); longer hop -> 0x67300 from the previous cell to
// the waypoint (zero fails). Returns 1 when every hop is clear.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Common_67700_CheckStoredRouteClear(uint32_t pHolder)
{
	const uint32_t pPath = W(pHolder, 0x30);
	const uint32_t pRoom = W(pPath, 0x1C);
	const int32_t nCount = static_cast<int32_t>(W(pPath, 0x28));
	const uint32_t nPattern = W(pPath, 0x48);
	const uint32_t nMask = W(pPath, 0x50);
	uint32_t prev = W(pHolder, 0);
	for (int32_t i = 0; i < nCount; ++i)
	{
		const uint32_t wp = W(pPath, 0x9C + static_cast<uint32_t>(i) * 4);
		int32_t dx = static_cast<int32_t>(wp & 0xFFFF) - static_cast<int32_t>(prev & 0xFFFF);
		if (dx < 0) dx = -dx;
		bool bShort = false;
		if (dx < 2)
		{
			int32_t dy = static_cast<int32_t>(wp >> 16) - static_cast<int32_t>(prev >> 16);
			if (dy < 0) dy = -dy;
			bShort = dy < 2;
		}
		if (bShort)
		{
			if (d2_port8_4CE90_Collision(pRoom, static_cast<int32_t>(wp & 0xFFFF), static_cast<int32_t>(wp >> 16), nPattern, nMask) != 0)
				return 0;
		}
		else if (d2_fn_d2common_67300(P(pPath), prev, wp) == 0)
			return 0;
		prev = wp;
	}
	return 1;
}
