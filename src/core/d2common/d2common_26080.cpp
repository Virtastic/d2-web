// 1.13c D2Common+0x26080 (#10366): stdcall(unit), the total of stat 0xC2 (layer 0) on an item's stat list.
// Retail 32-bit layout, read directly (wasm64 pointers are twice as wide as the retail fields):
//   unit +0x00 type, +0x5C stat list; data tables (sgptDataTables, passed as dt) +0xBCC itemstatcost base,
//   +0xBD4 itemstatcost count; the 0xC2 record sits at base + 0xF588.
// A null unit is fatal 0xEA7, a non-item fatal 0xEA8 (both answer 0 here). For an item with a stat list, a count
// above 0xC2 and a non-null record, the answer is the 0x38A80 total (EBX = list, EDI = 0x00C20000, record on the
// stack); otherwise 0.
//
// 0x38A80 (statlist total), 32-bit layout: list +0x10 flags (bit 31 = extended), +0x24 / +0x48 stat arrays
// {ptr, int16 count} (the second when extended), +0x44 a pointer to a mode dword. The array is binary-searched
// (0x382B0) for the key, entries {key, value} of 8 bytes. A hit's value is raised to record+0x2C shifted by
// record+0x18 when the record's byte +5 has bit 2 set, the list is extended, and the mode pointer is set with
// mode 0 or 1 and the value is below record+0x2C. A miss is 0.
#include <cstdint>
#include <cstring>

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
uint8_t R8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(a)); }
int16_t R16(uint32_t a) { int16_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 2); return v; }

// 0x382B0: EAX = array header {ptr, int16 count}, EDI = key; the index or -1.
int32_t FindStat(uint32_t arr, int32_t key)
{
	int32_t hi = R16(arr + 4);
	int32_t lo = 0;
	if (hi <= 0)
		return -1;
	int32_t mid;
	do {
		mid = (hi - lo) / 2 + lo;
		const int32_t k = static_cast<int32_t>(R32(R32(arr) + static_cast<uint32_t>(mid) * 8));
		if (k < key) {
			lo = mid + 1;
			mid = hi;
		} else if (k <= key) {
			return mid;
		}
		hi = mid;
	} while (lo < mid);
	return -1;
}

// 0x38A80: EBX = list, EDI = key, stack = record.
int32_t StatTotal(uint32_t list, int32_t key, uint32_t rec)
{
	const uint32_t ext = R32(list + 0x10) & 0x80000000u;
	const uint32_t arr = ext ? list + 0x48 : list + 0x24;
	const int32_t idx = FindStat(arr, key);
	if (idx < 0)
		return 0;
	const uint32_t entry = R32(arr) + static_cast<uint32_t>(idx) * 8;
	if (!entry)
		return 0;
	int32_t value = static_cast<int32_t>(R32(entry + 4));
	if (rec && (R8(rec + 5) & 4) && ext) {
		const uint32_t mode = R32(list + 0x44);
		if (mode && (R32(mode) == 0 || R32(mode) == 1)) {
			const int32_t floor = static_cast<int32_t>(R32(rec + 0x2C));
			if (value < floor)
				value = static_cast<int32_t>(static_cast<uint32_t>(floor) << (R8(rec + 0x18) & 0x1F));
		}
	}
	return value;
}
}

extern "C" int32_t D2Common_26080_OpStatTotal32(uint32_t unit, uint32_t dt)
{
	if (!unit || R32(unit) != 4)
		return 0;   // fatal 0xEA7 / 0xEA8
	const uint32_t list = R32(unit + 0x5C);
	if (!list || static_cast<int32_t>(R32(dt + 0xBD4)) <= 0xC2)
		return 0;
	const uint32_t rec = R32(dt + 0xBCC) + 0xF588;
	if (!rec)
		return 0;
	return StatTotal(list, 0x00C20000, rec);
}

// Shared with 0x266E0: the 0x38A80 total on a 32-bit-layout list.
extern "C" int32_t D2Common_38A80_StatTotal32(uint32_t list, int32_t key, uint32_t rec) { return StatTotal(list, key, rec); }
