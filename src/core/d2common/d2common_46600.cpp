// D2Common 0x46600 (#10699), written from the 1.13c disassembly. Zeroes both outputs, then needs an item unit (type 4), a skill
// id below the skills count (dt +0xBA0) and a level 1..0x3F. The layer is (skill << dt[+0xC6C]) + (level & dt[+0xC70]). The
// 0x40-flagged stat lists from 0x384F0 are asked (0x38B00, stat 0xCC, only when the stat count dt +0xBD4 exceeds 0xCC and the
// row is not null) for that layer; the walk follows +0x2C to the next list with flag bit 0x40 at +0x10. A hit writes the value
// & 0xFF and the list and returns 1. Otherwise each item of the inventory at +0x60 (0x1E190 / 0x1E8F0) is searched the same
// way (a non-item entry recurses with a null unit).
#include <cstdint>
#include "d2common_46600.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline uint8_t rb(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

// 1.13c D2Common+0x46600
extern "C" uint32_t D2Common_46600_With(uint32_t unit, int32_t skill, int32_t level, uint32_t pValue, uint32_t pList,
	const D2Common46600Env* e)
{
	if (pValue) wr(pValue, 0);
	if (pList) wr(pList, 0);
	if (!unit || rd(unit) != 4) return 0;
	const uint32_t dt = e->dataTables;
	if (skill < 0 || skill >= int32_t(rd(dt + 0xba0)) || level <= 0 || level > 0x3f) return 0;
	const uint32_t layer = (uint32_t(skill) << (rd(dt + 0xc6c) & 31)) + (rd(dt + 0xc70) & uint32_t(level));
	for (uint32_t l = e->statList(unit, 0x40); l; ) {
		if (int32_t(rd(dt + 0xbd4)) > 0xcc) {
			const uint32_t row = rd(dt + 0xbcc) + 0x10230;
			if (row) {
				const uint32_t v = e->getStat(l, 0xcc0000u + (layer & 0xffff), row);
				if (v) {
					if (pValue) wr(pValue, v & 0xff);
					if (pList) wr(pList, l);
					return 1;
				}
			}
		}
		uint32_t n = rd(l + 0x2c);
		while (n && !(rb(n + 0x10) & 0x40)) n = rd(n + 0x2c);
		l = n;
	}
	const uint32_t inv = rd(unit + 0x60);
	if (!inv) return 0;
	for (uint32_t it = e->first(inv); it; it = e->next(it)) {
		if (D2Common_46600_With(rd(it) == 4 ? it : 0, skill, level, pValue, pList, e)) return 1;
	}
	return 0;
}
