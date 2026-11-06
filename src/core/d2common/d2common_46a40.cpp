// D2Common 0x46A40 (#10890), written from the 1.13c disassembly. A null unit is fatal (line 0x706). An item (type 4) with item
// data at +0x14 gets flag 0x400000 at data +0x18. With item-type flag 0x2D (0x24430) the stats 0x15, 0x16, 0x17, 0x18, 0x9F and
// 0xA0 of the stat list at +0x5C are each raised to 3/2 of their value (signed, truncating); without it only stat 0x1F. Per
// stat: the value is read through 0x38B00 only when the stat count (dt +0xBD4) exceeds the stat id, its row (dt +0xBCC + id *
// 0x144) is not null and the list is not null (else 0); 0x3A280 stores it (layer 0) and, when that succeeds on a player (type
// 0), 0x38550 marks the stat. The result is the last store's (or mark's) EAX.
#include <cstdint>
#include "d2common_46a40.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

static uint32_t raise(uint32_t unit, uint32_t stat, const D2Common46A40Env* e)
{
	const uint32_t dt = e->dataTables;
	uint32_t v = 0;
	if (int32_t(rd(dt + 0xbd4)) > int32_t(stat)) {
		const uint32_t row = rd(dt + 0xbcc) + stat * 0x144u;
		const uint32_t list = rd(unit + 0x5c);
		if (row && list) v = e->getStat(list, stat << 16, row);
	}
	const int32_t t = int32_t(v * 3u);
	const uint32_t r = e->setStat(rd(unit + 0x5c), stat, uint32_t((t + (t < 0 ? 1 : 0)) >> 1), 0);
	if (r && rd(unit) == 0) return e->update(stat << 16, rd(unit + 0x5c));
	return r;
}

// 1.13c D2Common+0x46A40
extern "C" uint32_t D2Common_46A40_With(uint32_t unit, const D2Common46A40Env* e)
{
	if (!unit) { e->fatal(0x706); return 0; }
	if (rd(unit) == 4) { const uint32_t d = rd(unit + 0x14); if (d) wr(d + 0x18, rd(d + 0x18) | 0x400000u); }
	if (!e->flag(unit, 0x2d)) return raise(unit, 0x1f, e);
	static const uint32_t st[6] = { 0x15, 0x16, 0x17, 0x18, 0x9f, 0xa0 };
	uint32_t r = 0;
	for (uint32_t s : st) r = raise(unit, s, e);
	return r;
}
