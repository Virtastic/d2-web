// D2Common 0x49340 (ECX, EDX, stack s0..s8; ret 0x24). Written from the 1.13c disassembly: s0 = unit, s1 = record, s5 = amount
// (0: rolled by 0x465B0). A non-item unit gets stat 0x12 and 0x11 by the amount (returns 1). An item with a type row
// (0x45A40) resolves its items.txt type twice through 0x719A0 + the link table: the first sets the base minimums 0x15/0x17
// (0x9F when 0x24C30 holds) from type-row bytes +0xFE/+0x102/+0x100, the second 0x16/0x18/0xA0 from +0xFF/+0x103/+0x101 --
// both only with flag 0x2D. Three further lookups are made and dropped (each result overwrites the s0 argument slot, never read
// again); a sixth resolves the item record once more. Then, with flag 0x2D and amount * max(+0x103, +0xFF) / 100 + max <= max, the call
// hands over to 0x48630 (ECX and EDX = the caller's, saved at entry; amount 1); otherwise stats 0x12 and 0x11 get the amount, returned.
// A missing item record is fatal (line 0x61F); a unit that stops being an item between lookups, 0x61C.
#include <cstdint>
#include "d2common_49340.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline uint8_t rb(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }

static void mins(uint32_t unit, uint32_t typeRows, int32_t idx, const uint32_t off[3], const uint32_t st[3],
	const D2Common49340Env* e)
{
	if (idx < 0) return;
	const uint32_t t = uint32_t(idx) * 0x1a8u + typeRows;
	if (!t) return;
	if (!e->flag(unit, 0x2d)) return;
	if (rb(t + off[0])) e->setBase(unit, st[0], rb(t + off[0]), 0);
	if (rb(t + off[1])) e->setBase(unit, st[1], rb(t + off[1]), 0);
	if (e->cond(unit) && rb(t + off[2])) e->setBase(unit, st[2], rb(t + off[2]), 0);
}

// 1.13c D2Common+0x49340
extern "C" uint32_t D2Common_49340_With(uint32_t ecx, uint32_t edx, uint32_t unit, uint32_t rec, uint32_t s2, uint32_t s3,
	uint32_t amount, uint32_t s6, uint32_t s7, uint32_t s8, const D2Common49340Env* e)
{
	if (!rec) return 0;
	if (!amount) amount = e->roll(rd(rec + 0xC), rd(rec + 8), unit);
	if (!unit || rd(unit) != 4) {
		e->apply(rec, edx, unit, s2, 0x12, 0, amount, s6, s7);
		e->apply(rec, edx, unit, s2, 0x11, 0, amount, s6, s7);
		return 1;
	}
	const uint32_t row = e->typeRow(edx, unit);
	if (!row) return 0;
	static const uint32_t offA[3] = { 0xfe, 0x102, 0x100 }, stA[3] = { 0x15, 0x17, 0x9f };
	static const uint32_t offB[3] = { 0xff, 0x103, 0x101 }, stB[3] = { 0x16, 0x18, 0xa0 };
	for (int i = 0; i < 6; i++) {
		if (rd(unit) != 4) { e->fatal(0x61c); return 0; }
		const uint32_t ir = e->itemRec(rd(unit + 4));
		if (!ir) { e->fatal(0x61f); return 0; }
		const int32_t idx = e->link(e->linkTable, rd(ir + 0x80), 0);
		if (i == 0) mins(unit, e->typeRows, idx, offA, stA, e);
		else if (i == 1) mins(unit, e->typeRows, idx, offB, stB, e);
	}
	const uint32_t b103 = rb(row + 0x103), bff = rb(row + 0xff);
	const uint32_t mx = int32_t(b103) > int32_t(bff) ? b103 : bff;
	if (e->flag(unit, 0x2d)) {
		const int32_t q = e->mulDiv(100, amount, mx);
		if (!(int32_t(uint32_t(q) + mx) > int32_t(mx)))
			return e->dmg(ecx, edx, unit, rec, s2, s3, 0, 1, s6, s7, s8);
	}
	e->apply(rec, edx, unit, s2, 0x12, 0, amount, s6, s7);
	e->apply(rec, edx, unit, s2, 0x11, 0, amount, s6, s7);
	return amount;
}
