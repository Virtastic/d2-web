#include <test_seam.h>
#include "ITEMS/ItemsRetail113.h"

#include <cstring>

// Retail-layout (1.13c) item routines. Addresses are the 32-bit retail ones; the D2Common ordinals are declared in the header
// and trap here (the game has no retail-layout caller). Written from the 1.13c decompile; one body per marked function.

#define D2R_TRAP() __builtin_trap()
uint32_t D2Retail113_Ord10695_ItemClassRecord(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10718_ItemFormat(uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_Ord10086_ItemLevelCap(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10411_ItemSeed(uint32_t) { D2R_TRAP(); }
void D2Retail113_Ord10536_SetItemRow(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_Ord10977_ItemFlag(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_Ord10996_ItemEvent(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10926_Table() { D2R_TRAP(); }
uint32_t D2Retail113_Roll1180(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Roll11D0(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10865_ItemFlag(uint32_t) { D2R_TRAP(); }
void D2Retail113_Ord10887_SetStat(uint32_t, uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10744_IsType(uint32_t, uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_Ord10587_GetStat(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10711_ItemFlag(uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_Ord10910_GetBaseStat(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_Sub13230_Scale(int32_t, int32_t, int32_t) { D2R_TRAP(); }
void D2Retail113_Sub13DE0_Finish(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10295_SlotTable() { D2R_TRAP(); }
uint32_t D2Retail113_Ord11007_SlotType(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord11094_TypeAccepts(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10220_PickTable() { D2R_TRAP(); }
uint32_t D2Retail113_Ord10110_ElementAccepts(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_DataTables() { D2R_TRAP(); }
uint32_t D2Retail113_Sub13A30_NoFormat() { D2R_TRAP(); }
uint32_t D2Retail113_SgptRowTable_B39C() { D2R_TRAP(); }
uint32_t D2Retail113_UnitSeed_A5EC(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_UnitWord_B37E(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_UnitByte_B390(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_UnitLevel_A8F8(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_MonRecord_A53E(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_UnitKind_A898(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_RowAllowed_B3C0(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_UnitDiff_B3D8(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_RowBlocked_131E0(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_Announce_A9D0(uint32_t, uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }

namespace
{
inline uint32_t Rd32(uint32_t a, uint32_t o = 0) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
inline uint16_t Rd16(uint32_t a, uint32_t o = 0) { uint16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
inline uint8_t Rd8(uint32_t a, uint32_t o = 0) { return *((const uint8_t*)(uintptr_t)a + o); }
inline int32_t Pct75(int32_t v) { return (int32_t)((uint32_t)v * 0x4Bu) / 100; }

// the item seed step (x * 0x6AC690C5 + hi): stores the new pair, returns its low word
inline uint32_t StepSeed(uint32_t seed)
{
	const uint64_t p = (uint64_t)Rd32(seed, 0) * 0x6AC690C5u + Rd32(seed, 4);
	std::memcpy((uint8_t*)(uintptr_t)seed, &p, 8);
	return (uint32_t)p;
}

// 0x14210 / 0x14370 share one rule over the #10220 table
uint32_t PickFromTable(uint32_t unit, uint32_t flag)
{
	const uint32_t p = D2_CALL(D2Retail113_Ord10220_PickTable);
	const uint32_t cur = flag ? Rd32(p, 0xC) : Rd32(p, 8);
	if (!cur) return (flag ? p : cur) & 0xffff0000u;
	const uint32_t base = Rd32(p, 4);
	const int32_t span = flag ? (int32_t)(Rd32(p, 0) * 0x48u - Rd32(p, 0xC) + base) : (int32_t)(Rd32(p, 0xC) - Rd32(p, 8));
	const int32_t n = span / 0x48;
	const int32_t first = (int32_t)(cur - base) / 0x48;
	uint32_t pos[0x1FF];
	uint32_t got = 0, el = cur;
	for (int32_t j = 0; j < n; ++j, el += 0x48)
		if (D2_CALL(D2Retail113_Ord10110_ElementAccepts, unit, el) && (int32_t)got < 0x1FF) pos[got++] = (uint32_t)(first + j);
	if (n <= 0) return (uint32_t)span * 0x38E38E39u & 0xffff0000u;
	if (!got) return 0;
	const uint32_t seed = D2_CALL(D2Retail113_Ord10411_ItemSeed, unit);
	const uint32_t nlo = StepSeed(seed);
	const uint32_t r = (got & (got - 1)) ? nlo % got : (nlo & (got - 1));
	return pos[r] + 1;
}
}

// 1.13c D2Game+0x13C20 (stdcall item, client; weighted drop-table roll over the D2Common table at tables+0xC18, stride 0x1B8)
uint32_t D2Retail113_13C20(uint32_t item, uint32_t client)
{
	int32_t chosen = -1;
	int32_t pairs[0x200];
	int32_t n = 0; uint32_t total = 0;
	const uint32_t rec = D2_CALL(D2Retail113_Ord10695_ItemClassRecord, item ? Rd32(item, 4) : 0xFFFFFFFFu);
	if (rec == 0) return 0;
	const uint32_t fmt = D2_CALL(D2Retail113_Ord10718_ItemFormat, item);
	const uint32_t recId = Rd32(rec, 0x80);
	const int32_t level = D2_CALL(D2Retail113_Ord10086_ItemLevelCap, item);
	const uint32_t tables = D2_CALL(D2Retail113_DataTables);
	const int32_t rows = (int32_t)Rd32(tables, 0xC1C);
	const uint32_t table = Rd32(tables, 0xC18);
	if (rows <= 0) return 0;
	for (int32_t i = 0; i < rows; ++i)
	{
		const uint32_t row = table + (uint32_t)i * 0x1B8u;
		if (Rd16(row, 0x22) >= 100 && (uint16_t)fmt < 100) continue;
		if ((int32_t)(int16_t)Rd16(row, 0x30) > level) continue;
		if (recId != Rd32(row, 0x28)) continue;
		if (Rd16(row, 0x2C) == 0x1D && !(Rd8(client, 0x80) & 1)) continue;
		const uint32_t pick = Rd32(client, 0x40);
		if (pick != 0 && pick - 1 == (uint32_t)i) chosen = i;
		const uint32_t w = Rd32(row, 0x34);
		pairs[n * 2] = i; pairs[n * 2 + 1] = w ? (int32_t)w : 1;
		total += (uint32_t)pairs[n * 2 + 1];
		++n;
	}
	if (chosen < 0)
	{
		if (total == 0) return 0;
		uint32_t roll = 0;
		if ((int32_t)total > 0)
		{
			const uint32_t p = StepSeed(D2_CALL(D2Retail113_Ord10411_ItemSeed, item));
			roll = (total & (total - 1)) ? p % total : (total - 1) & p;
		}
		int32_t k = 0;
		if (n <= 0) return 0;
		while (pairs[k * 2 + 1] <= (int32_t)roll)
		{
			roll -= (uint32_t)pairs[k * 2 + 1];
			if (++k >= n) return 0;
		}
		chosen = pairs[k * 2];
	}
	D2_CALL(D2Retail113_Ord10536_SetItemRow, item, (uint32_t)chosen);
	D2_CALL(D2Retail113_Ord10977_ItemFlag, item, 0x10, 0);
	D2_CALL(D2Retail113_Ord10996_ItemEvent, 4, 0, item, 0, 0, 0);
	return 1;
}

// 1.13c D2Game+0x13E60 (edi = item, stack = arg, ret 4; second-layout item roll)
int32_t D2Retail113_13E60(uint32_t item, uint32_t arg)
{
	const uint32_t g = D2_CALL(D2Retail113_Ord10926_Table);
	const uint32_t cls = item ? Rd32(item, 4) : 0xFFFFFFFFu;
	const uint32_t rec = D2_CALL(D2Retail113_Ord10695_ItemClassRecord, cls);
	if (g == 0 || rec == 0) return 0;
	D2_CALL(D2Retail113_Ord10536_SetItemRow, item, D2_CALL(D2Retail113_Roll1180, D2_CALL(D2Retail113_Ord10411_ItemSeed, item), Rd32(g, 0)));
	int32_t ret = 0;
	if (D2_CALL(D2Retail113_Ord10865_ItemFlag, item))
	{
		const uint32_t rec2 = D2_CALL(D2Retail113_Ord10695_ItemClassRecord, item ? Rd32(item, 4) : 0xFFFFFFFFu);
		if (rec2)
		{
			int32_t v = (int32_t)(uint32_t)(Rd8(rec2, 0x112) * 0x21u) / 100;
			if (v < 1) v = 1;
			const uint32_t half = (uint32_t)v >> 1;
			uint32_t extra = D2_CALL(D2Retail113_Roll11D0, item + 0x20, half) + half;
			if (extra == 0) extra = 1;
			D2_CALL(D2Retail113_Ord10887_SetStat, item, 0x48, extra, 0);
			D2_CALL(D2Retail113_Ord10887_SetStat, item, 0x49, (uint32_t)v, 0);
		}
	}
	if (D2_CALL(D2Retail113_Ord10744_IsType, item, 0x2D))
	{
		static const uint32_t stat[4] = { 0x16, 0x15, 0x18, 0x17 };
		static const int32_t floorv[4] = { 2, 1, 2, 1 };
		for (int i = 0; i < 4; ++i)
		{
			int32_t q = Pct75(D2_CALL(D2Retail113_Ord10587_GetStat, item, stat[i], 0));
			if (q < floorv[i]) q = floorv[i];
			D2_CALL(D2Retail113_Ord10887_SetStat, item, stat[i], (uint32_t)q, 0);
		}
		if (D2_CALL(D2Retail113_Ord10711_ItemFlag, item))
		{
			D2_CALL(D2Retail113_Ord10887_SetStat, item, 0x9F, (uint32_t)D2_CALL(D2Retail113_Sub13230_Scale, D2_CALL(D2Retail113_Ord10910_GetBaseStat, item, 0x9F, 0), 2, 0x4B), 0);
			D2_CALL(D2Retail113_Ord10887_SetStat, item, 0xA0, (uint32_t)D2_CALL(D2Retail113_Sub13230_Scale, D2_CALL(D2Retail113_Ord10910_GetBaseStat, item, 0xA0, 0), 1, 0x4B), 0);
		}
		ret = 1;
	}
	else if (D2_CALL(D2Retail113_Ord10744_IsType, item, 0x32))
	{
		int32_t q = Pct75(D2_CALL(D2Retail113_Ord10587_GetStat, item, 0x1F, 0));
		if (q <= 0) q = 1;
		D2_CALL(D2Retail113_Ord10887_SetStat, item, 0x1F, (uint32_t)q, 0);
		ret = 1;
	}
	D2_CALL(D2Retail113_Sub13DE0_Finish, item, arg);
	return ret;
}

// 1.13c D2Game+0x140B0 (stdcall item, arg; picks an item-type slot with the item seed)
uint32_t D2Retail113_140B0(uint32_t item, uint32_t arg)
{
	const uint32_t tbl = D2_CALL(D2Retail113_Ord10295_SlotTable);
	const uint32_t rec = D2_CALL(D2Retail113_Ord10695_ItemClassRecord, item ? Rd32(item, 4) : 0xFFFFFFFFu);
	if (!tbl || !rec) return 0;
	int32_t n = (int32_t)Rd32(tbl, 0);
	uint8_t tried[64] = {};
	if (D2_CALL(D2Retail113_Ord10711_ItemFlag, item) || Rd8(rec, 0x113)) n = 4;
	for (;;)
	{
		uint32_t slot = 0;
		do
		{
			const uint32_t seed = D2_CALL(D2Retail113_Ord10411_ItemSeed, item);
			if (n > 0)
			{
				const uint32_t nlo = StepSeed(seed);
				slot = ((uint32_t)n & ((uint32_t)n - 1)) ? nlo % (uint32_t)n : (nlo & ((uint32_t)n - 1));
			}
			else slot = 0;
		} while (tried[slot]);
		const uint32_t type = D2_CALL(D2Retail113_Ord11007_SlotType, slot);
		if (!type) return 0;
		if (D2_CALL(D2Retail113_Ord11094_TypeAccepts, item, type))
		{
			D2_CALL(D2Retail113_Ord10536_SetItemRow, item, slot);
			D2_CALL(D2Retail113_Ord10996_ItemEvent, 1, 0, item, type, 0, 0);
			D2_CALL(D2Retail113_Sub13DE0_Finish, item, arg);
			return 1;
		}
		tried[slot] = 1;
		int32_t i = 0;
		if (n <= 0) return 0;
		while (tried[i]) if (++i >= n) return 0;
	}
}

// 1.13c D2Game+0x141E0 (eax = item, stack = arg; item format dispatch)
uint32_t D2Retail113_141E0(uint32_t item, uint32_t arg)
{
	if ((uint16_t)D2_CALL(D2Retail113_Ord10718_ItemFormat, item) >= 1) return D2_CALL(D2Retail113_13C20, item, arg);
	return D2_CALL(D2Retail113_Sub13A30_NoFormat);
}

// 1.13c D2Game+0x14210 (stdcall unit, flag; table pick over #10220's 0x48-byte elements)
uint32_t D2Retail113_14210(uint32_t unit, uint32_t flag)
{
	return PickFromTable(unit, flag);
}

// 1.13c D2Game+0x14370 (stdcall unit, flag; the same pick over the second table copy)
uint32_t D2Retail113_14370(uint32_t unit, uint32_t flag)
{
	return PickFromTable(unit, flag);
}

// 1.13c D2Game+0x144D0 (seven stack words, ret 0x1C; weighted pick of a row from the 0x90-stride table for the unit's level)
uint32_t D2Retail113_144D0(uint32_t unit, uint32_t needFlag, uint32_t bypass, uint32_t announce, uint32_t second, int32_t forced, uint32_t idFilter)
{
	const uint32_t t = D2_CALL(D2Retail113_SgptRowTable_B39C);
	if (!t) return 0;
	uint32_t lo, hi;
	if (idFilter) { lo = Rd32(t, 0x10); hi = Rd32(t, 0) * 0x90u + Rd32(t, 4); }
	else if (second) { lo = Rd32(t, 0xC); hi = Rd32(t, 0x10); }
	else { lo = Rd32(t, 8); hi = Rd32(t, 0xC); }
	const int32_t cnt = (int32_t)(hi - lo) / 0x90;
	if (!lo) return (uint32_t)cnt & 0xFFFF0000u;
	const int32_t base = (int32_t)(lo - Rd32(t, 4)) / 0x90;
	uint32_t seed = D2_CALL(D2Retail113_UnitSeed_A5EC, unit);
	const uint64_t v = (uint64_t)Rd32(seed, 0) * 0x6AC690C5ull + Rd32(seed, 4);
	std::memcpy((uint8_t*)(uintptr_t)seed, &v, 8);
	const uint32_t w = D2_CALL(D2Retail113_UnitWord_B37E, unit) & 0xFFFF;
	const int32_t ul = (int32_t)(D2_CALL(D2Retail113_UnitByte_B390, unit) & 0xFF);
	if (!(v & 1) && !bypass) return 0;
	int32_t lv = (int32_t)D2_CALL(D2Retail113_UnitLevel_A8F8, unit);
	const uint32_t m = D2_CALL(D2Retail113_MonRecord_A53E, unit ? Rd32(unit, 4) : 0xFFFFFFFFu);
	if (!m) return 0;
	if (lv <= ul) lv = ul;
	const int32_t f = (int32_t)Rd8(m, 0x140);
	if (f) lv += f;
	else
	{
		const int32_t h = ul / 2, c = 99 - h;
		lv = lv < c ? lv - h : 2 * lv - h - c;
	}
	lv = lv <= 1 ? 1 : lv < 99 ? lv : 99;
	if (cnt <= 0) return (uint32_t)cnt & 0xFFFF0000u;
	static uint32_t cand[0x1FF][2];
	int32_t n = 0;
	uint32_t sum = 0, eaxLeft = (uint32_t)cnt;
	int32_t idx = -1; uint32_t pick = 0; bool have = false;
	for (int32_t i = 0; i < cnt; i++)
	{
		const uint32_t row = lo + (uint32_t)i * 0x90u;
		if (needFlag && !Rd8(row, 0x54)) continue;
		if ((uint32_t)Rd16(row, 0x22) >= 100 && w < 100) continue;
		const bool isForced = forced > 0 && forced - 1 == i;
		if (!isForced)
		{
			if ((int32_t)Rd32(row, 0x58) > lv) continue;
			const int32_t mx = (int32_t)Rd32(row, 0x60);
			if (mx && mx < lv) continue;
		}
		if (!Rd8(row, 0x64))
		{
			const int32_t k = (int32_t)D2_CALL(D2Retail113_UnitKind_A898, unit);
			if (k == 6 || (k > 7 && k <= 9)) continue;
		}
		if (!D2_CALL(D2Retail113_RowAllowed_B3C0, unit, row)) continue;
		if (idFilter && idFilter != Rd32(row, 0x5C)) continue;
		if (!Rd8(row, 0x82)) continue;
		if (Rd8(row, 0x66) != 0xFF && D2_CALL(D2Retail113_UnitDiff_B3D8, unit) != 7 && Rd8(row, 0x66) != D2_CALL(D2Retail113_UnitDiff_B3D8, unit)) continue;
		if (D2_CALL(D2Retail113_RowBlocked_131E0, unit, Rd32(row, 0x5C))) continue;
		if (isForced)
		{
			if (i + base >= 0) { idx = i + base; pick = row; have = true; }
			else eaxLeft = row;
			break;
		}
		if (n < 0x1FF) { cand[n][0] = (uint32_t)(i + base); cand[n][1] = row; n++; }
		sum += f ? Rd8(row, 0x82) * Rd32(row, 0x58) : Rd8(row, 0x82);
	}
	if (!have)
	{
		if (n <= 0) return eaxLeft & 0xFFFF0000u;
		seed = D2_CALL(D2Retail113_UnitSeed_A5EC, unit);
		int32_t left = 0;
		{
			const uint32_t cnt2 = sum + 1;
			if ((int32_t)cnt2 > 0)
			{
				const uint64_t vv = (uint64_t)Rd32(seed, 0) * 0x6AC690C5ull + Rd32(seed, 4);
				std::memcpy((uint8_t*)(uintptr_t)seed, &vv, 8);
				left = (int32_t)((uint32_t)vv % cnt2);
			}
		}
		int32_t j = 0;
		for (; j < n; j++)
		{
			const uint32_t row = cand[j][1];
			left -= (int32_t)(f ? Rd8(row, 0x82) * Rd32(row, 0x58) : Rd8(row, 0x82));
			if (left < 0) break;
		}
		if (j == n) j = n - 1;
		idx = (int32_t)cand[j][0]; pick = cand[j][1];
	}
	if (announce) D2_CALL(D2Retail113_Announce_A9D0, 0, 0, unit, pick);
	return (uint32_t)(idx + 1);
}
