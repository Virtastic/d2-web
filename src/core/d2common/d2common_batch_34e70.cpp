// 1.13c D2Common implementation batch: +0x34E70, +0x356E0, +0x37170, +0x38940.
// Bodies translated from the pinned 1.13c decompiles. Structs with a vendored twin are read by name;
// retail room coordinates live at +0x4C and keep their 1.13c offsets. Callees that are still
// UNMAPPED rows are env-seam function pointers (null until the environment installs them); fatal
// paths (Fog message + _exit(-1)) are modelled as returns, as in the other d2common batch files.
#include <cstddef>
#include <cstring>
#include <cstdint>

#include <emscripten.h>

#include "D2BitManip.h"
#include "D2Monsters.h"
#include "D2StatList.h"
#include "DataTbls/ItemsTbls.h"
#include "Drlg/D2DrlgDrlg.h"
#include "Path/Path.h"

// The vendored path/stat-list structs carry our engine's 8-byte pointers, so their offsets differ
// from retail's; the 1.13c offsets below are matched to fields by name (retail room coordinates
// 0x4C..0x58 are D2DrlgCoordsStrc's subtile box, path 0x28/0x34/0x9C the count, flags and points).
static_assert(offsetof(D2DynamicPathStrc, pRoom) == 0x1C, "path pRoom");
static_assert(offsetof(D2ItemStatCostTxt, nKeepZero) == 0x50, "itemstatcost keep zero");
static_assert(offsetof(D2ItemStatCostTxt, bHasOpApplyingToItem) == 0x53, "itemstatcost op-to-item flag");

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t Rd32(const void* b, int o)
	{
		uint32_t v;
		__builtin_memcpy(&v, static_cast<const uint8_t*>(b) + o, sizeof(v));
		return v;
	}
	inline void Wr32(void* b, int o, uint32_t v)
	{
		__builtin_memcpy(static_cast<uint8_t*>(b) + o, &v, sizeof(v));
	}
}

// Retail .data tables shared with the +0x37920/+0x37CA0 rows: six 3-dword records each, [0] scaled
// count, [1] value. Defined and filled by the module's loader (d2common_batch_37ca0.cpp).
extern "C"
{
	extern uint32_t g_d2common_dat_6fdeb160[18];
	extern uint32_t g_d2common_dat_6fdeb1f0[18];
}

// Env seams for callees that are still-UNMAPPED rows. The decompile shows no visible arguments for
// either call; the 1.13c register set-up at the call site is not recovered yet.
extern "C"
{
	// 0x1330 room at a point: EAX = a start room, EBX = x, EDI = y; returns the room or 0.
	uint32_t (*g_d2common_fn_1330)(uint32_t eax, uint32_t ebx, uint32_t edi) = nullptr;
	// 0x2D320 DRLG_IsTownLevel: EAX = level id.
	int32_t (*g_d2common_fn_2D320)(uint32_t eax) = nullptr;
}

// 1.13c D2Common+0x34E70: keep PATH flag bit 0 clear while every path point lies inside the path's
// current room. The flag is cleared first; with at least one point the room (path+0x1C) must exist
// and each point's X/Y (words at 0x9C/0x9E + i*4) must satisfy x in [room+0x4C, +0x4C+0x54) and
// y in [room+0x50, +0x50+0x58); the first point outside the box, or a missing room, sets bit 0.
// A null path is fatal in retail; we return.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_port_d2common_34e70(uint32_t pPath)
{
	uint8_t* path = P(pPath);
	if (!path)
		return;

	// Retail offsets throughout: the vendored struct's pointer width differs from retail's 32-bit layout.
	Wr32(path, 0x34, Rd32(path, 0x34) & ~1u);
	if (static_cast<int32_t>(Rd32(path, 0x28)) <= 0)
		return;

	const uint32_t nRoomAddr = Rd32(path, 0x1C);
	const uint8_t* room = nRoomAddr ? P(nRoomAddr) : nullptr;
	int32_t i = 0;
	for (;;)
	{
		if (!room)
			break;

		uint16_t nX16, nY16;
		__builtin_memcpy(&nX16, path + 0x9C + i * 4, 2);
		__builtin_memcpy(&nY16, path + 0x9E + i * 4, 2);
		const int32_t nX = static_cast<int32_t>(nX16);
		const int32_t nY = static_cast<int32_t>(nY16);
		const int32_t nRoomX = static_cast<int32_t>(Rd32(room, 0x4C));
		const int32_t nRoomY = static_cast<int32_t>(Rd32(room, 0x50));
		if (nX < nRoomX || nRoomX + static_cast<int32_t>(Rd32(room, 0x54)) <= nX)
			break;
		if (nY < nRoomY || nRoomY + static_cast<int32_t>(Rd32(room, 0x58)) <= nY)
			break;

		++i;
		if (static_cast<int32_t>(Rd32(path, 0x28)) <= i)
			return;
	}
	Wr32(path, 0x34, Rd32(path, 0x34) | 1u);
}

// 1.13c D2Common+0x356E0: town-movement predicate. Register map ESI = pThis (+0..+6 four u16 corner
// words, +8 the level index, +0xC an optional room), EDX = pUnit. Anything but a monster unit answers 1,
// as does a town-eligible monster (0x1A660). Without a room at this+0xC it is looked up through 0x1330
// (EAX = this+8, EBX/EDI = the box midpoint; then EAX = that result, EBX = this+4, EDI = this+6; only the second
// result is kept, null answers 1). The room's level id (room+0x10, then +0x58, then +0x1D0) goes to 0x2D320
// (EAX): nonzero answers 0.
extern "C"
int32_t d2common_356e0_port(uint32_t pThis, uint32_t pUnit)
{
	if (!pUnit || Rd32(P(pUnit), 0) != 1 ||
	    MONSTERS_CanBeInTown(reinterpret_cast<D2UnitStrc*>(static_cast<uintptr_t>(pUnit))))
		return 1;
	const uint8_t* t = P(pThis);
	uint32_t room = Rd32(t, 0x0C);
	if (room == 0)
	{
		auto u16 = [&](int o) { uint16_t v; __builtin_memcpy(&v, t + o, 2); return static_cast<int32_t>(v); };
		auto half = [](int32_t d) { return (d - (d >> 31)) >> 1; };
		const int32_t x0 = u16(0), y0 = u16(2), x1 = u16(4), y1 = u16(6);
		const uint32_t mx = static_cast<uint32_t>(half(x1 - x0) + x0);
		const uint32_t my = static_cast<uint32_t>(half(y1 - y0) + y0);
		const uint32_t lvl = Rd32(t, 0x08);
		// EAX is not reloaded for the second call: it still holds the first call's result.
		const uint32_t first = g_d2common_fn_1330 ? g_d2common_fn_1330(lvl, mx, my) : 0;
		room = g_d2common_fn_1330 ? g_d2common_fn_1330(first, static_cast<uint32_t>(x1), static_cast<uint32_t>(y1)) : 0;
		if (room == 0)
			return 1;
	}
	const uint32_t id = Rd32(P(Rd32(P(Rd32(P(room), 0x10)), 0x58)), 0x1D0);
	return (g_d2common_fn_2D320 && g_d2common_fn_2D320(id) != 0) ? 0 : 1;
}

// 1.13c D2Common+0x37170: refresh a path/game record's value and scaled count from its act index.
// record+0x30 selects the table (0: 6fdeb160, else 6fdeb1f0); +0x04 takes entry index*3+1 and +0x08
// takes record+0x28 multiplied by entry index*3, where index is record+0x00.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_port_d2common_37170(uint32_t pRecord)
{
	uint8_t* rec = P(pRecord);
	if (!rec)
		return;

	const uint32_t nIndex = Rd32(rec, 0x00) * 3;
	const uint32_t* pTable =
	    Rd32(rec, 0x30) == 0 ? g_d2common_dat_6fdeb160 : g_d2common_dat_6fdeb1f0;
	Wr32(rec, 0x04, pTable[nIndex + 1]);
	Wr32(rec, 0x08, Rd32(rec, 0x28) * pTable[nIndex]);
}

// Store one value in a stat list's full stat array. The list is the hidden this: the stat array
// (pointer, u16 count, u16 capacity) sits at +0x48, the flags at +0x10, the value-change callback at
// +0x5C. The packed layer/stat id is found by a signed binary search over the array; a missing entry
// is only inserted when the new value is non-zero. A zero value for a row without the keep-zero flag
// removes the entry, otherwise the value is written and a row with the item-op flag sets the
// permanent bit on the list. When the value changed, the callback is set and the row's flag byte
// carries bit 8, the callback runs (unit, packed id, old, new). The list is read in the original
// 32-bit layout, so the array code is done here on raw offsets; growth and shrink of the array (the
// allocator) are not reproduced and the array is expected to stay inside its capacity.
namespace
{
	inline uint8_t* P38940(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline int32_t FindAt(const uint8_t* arr, int32_t nCount, int32_t nKey, bool bForInsert)
	{
		int32_t lo = 0, hi = nCount;
		const uint8_t* base = P38940(*reinterpret_cast<const uint32_t*>(arr));
		while (lo < hi)
		{
			const int32_t mid = (hi - lo) / 2 + lo;
			const int32_t k = *reinterpret_cast<const int32_t*>(base + mid * 8);
			if (k < nKey) lo = mid + 1;
			else if (k <= nKey) return bForInsert ? -1 : mid;
			else hi = mid;
		}
		return bForInsert ? lo : -1;
	}
}

extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2common_38940_port(uint32_t pStatList, uint32_t nLayerStatId,
                                                int32_t nNewValue, uint32_t pItemStatCost,
                                                uint32_t pUnit)
{
	uint8_t* sl = P38940(pStatList);
	const uint8_t* row = P38940(pItemStatCost);
	if (!sl || !row)
		return 0;

	uint8_t* arr = sl + 0x48;
	int16_t& nCount = *reinterpret_cast<int16_t*>(arr + 4);
	uint8_t* base = P38940(*reinterpret_cast<uint32_t*>(arr));
	const int32_t nKey = static_cast<int32_t>(nLayerStatId);

	int32_t nIndex = FindAt(arr, nCount, nKey, false);
	uint8_t* pStat = nIndex >= 0 ? base + nIndex * 8 : nullptr;
	if (!pStat)
	{
		if (nNewValue == 0)
			return 0xFFFFFFFFu;			// EAX is the failed search's -1
		const int32_t nAt = FindAt(arr, nCount, nKey, true);
		if (nAt < 0)
			return 0;
		if (nAt < nCount)
			memmove(base + nAt * 8 + 8, base + nAt * 8, static_cast<size_t>(nCount - nAt) * 8);
		pStat = base + nAt * 8;
		*reinterpret_cast<uint32_t*>(pStat) = nLayerStatId;
		*reinterpret_cast<int32_t*>(pStat + 4) = 0;
		nCount = static_cast<int16_t>(nCount + 1);
	}

	const int32_t nPreviousValue = *reinterpret_cast<int32_t*>(pStat + 4);
	if (nNewValue == 0 && row[0x50] == 0)
	{
		const int32_t nAt = static_cast<int32_t>((pStat - base) >> 3);
		if (nAt < nCount)
		{
			if (nAt < nCount - 1)
				memmove(pStat, pStat + 8, static_cast<size_t>(nCount - nAt) * 8 - 8);
			nCount = static_cast<int16_t>(nCount - 1);
		}
	}
	else
	{
		*reinterpret_cast<int32_t*>(pStat + 4) = nNewValue;
		if (row[0x53] != 0)
			*reinterpret_cast<uint32_t*>(sl + 0x10) |= 0x20000000u;
	}

	// EAX at return: the row pointer when the value did not change, else the callback pointer, or the callback's result when it ran.
	if (nPreviousValue == nNewValue)
		return pItemStatCost;
	const uint32_t pfCallback = *reinterpret_cast<uint32_t*>(sl + 0x5C);
	if (pfCallback != 0 && (row[5] & 8) != 0)
	{
		using Callback4 = uint32_t (*)(uint32_t, uint32_t, int32_t, int32_t);
		return reinterpret_cast<Callback4>(static_cast<uintptr_t>(pfCallback))(
		    pUnit, nLayerStatId, nPreviousValue, nNewValue);
	}
	return pfCallback;
}
